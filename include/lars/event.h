#pragma once

#include <functional>
#include <memory>
#include <vector>
#include <utility>
#include <mutex>

namespace lars{
  
  template <typename ... Args> class Event;
  
  class Observer{
  public:
    struct Base{ virtual ~Base(){} };
    
    Observer(){}
    Observer(Observer &&other) = default;
    template <typename L> Observer(L && l):data(new L(std::move(l))){ }
    
    Observer & operator=(const Observer &other) = delete;
    Observer & operator=(Observer &&other) = default;
    
    template <typename L> Observer & operator=(L && l){ 
      data.reset(new L(std::move(l))); 
      return *this; 
    }
    
    template <typename H,typename ... Args> void observe(Event<Args...> & event,const H &handler){
      data.reset(new typename Event<Args...>::Observer(event.createObserver(handler)));
    }
    
    void reset(){ data.reset(); }
    operator bool() const { return bool(data); }
    
  private:
    std::unique_ptr<Base> data;
  };
  
  template <typename ... Args> class Event{
  private:

    using Handler = std::function<void(const Args &...)>;
    using HandlerID = size_t;

    struct StoredHandler {
      HandlerID id;
      Handler callback;
    };

    using HandlerList = std::vector<StoredHandler>;
    using EventPointer = std::shared_ptr<const Event *>;
    using WeakEventPointer = std::weak_ptr<const Event *>;
    
    mutable HandlerID IDCounter = 0;
    mutable HandlerList observers;
    mutable std::mutex observerMutex;
    EventPointer self;

    HandlerID addHandler(Handler h)const{
      std::lock_guard<std::mutex> lock(observerMutex);
      observers.emplace_back(StoredHandler{IDCounter,h});
      return IDCounter++;
    }
    
    void eraseHandler(const HandlerID &id)const{
      std::lock_guard<std::mutex> lock(observerMutex);
      auto it = std::find_if(observers.begin(), observers.end(), [&](auto &o){ return o.id == id; });
      if (it != observers.end()) { observers.erase(it); }
    }
    
  public:
    
    struct Observer:public lars::Observer::Base{
      WeakEventPointer parent;
      HandlerID id;
      
      Observer(){}
      Observer(const EventPointer & _parent, HandlerID _id):parent(_parent), id(_id){
      }
      
      Observer(Observer &&other) = default;
      Observer(const Observer &other) = delete;
      
      Observer & operator=(const Observer &other) = delete;
      Observer & operator=(Observer &&other)=default;
      
      void observe(const Event &event, const Handler &handler){
        *this = event.createObserver(handler);
      }

      void reset(){
        if(auto p = parent.lock()){ 
          (*p)->eraseHandler(id); 
        }
        parent.reset();
      }
      
      ~Observer(){ reset(); }
    };
    
    Event():self(std::make_shared<const Event *>(this)){
      
    }
    
    Event(const Event &) = delete;

    Event(Event &&other){
      *this = std::move(other);
    }
    
    Event & operator=(const Event &) = delete;

    Event & operator=(Event &&other){
      self = std::move(other.self);
      *self = this;
      observers = std::move(other.observers);
      IDCounter = other.IDCounter;
      return *this;
    }
    
    void emit(Args ... args) const {
      observerMutex.lock();
      auto tmpObservers = observers;
      observerMutex.unlock();
      for(auto &observer: tmpObservers){
        observer.callback(args...);
      }
    }
    
    Observer createObserver(const Handler &h)const{
      return Observer(self, addHandler(h));
    }
    
    void connect(const Handler &h)const{
      addHandler(h);
    }
    
    void clearObservers(){
      std::lock_guard<std::mutex> lock(observerMutex);
      observers.clear();
    }
    
    size_t observerCount() const {
      std::lock_guard<std::mutex> lock(observerMutex);
      return observers.size();
    }

  };
  
}
