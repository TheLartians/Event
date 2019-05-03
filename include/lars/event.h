#pragma once

#include <functional>
#include <memory>
#include <vector>
#include <utility>
#include <mutex>
#include <algorithm>

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
      std::shared_ptr<Handler> callback;
    };

    using HandlerList = std::vector<StoredHandler>;
    
    struct Data {
      HandlerID IDCounter = 0;
      HandlerList observers;
      std::mutex observerMutex;
    };

    std::shared_ptr<Data> data;

    HandlerID addHandler(Handler h)const{
      std::lock_guard<std::mutex> lock(data->observerMutex);
      data->observers.emplace_back(StoredHandler{data->IDCounter,std::make_shared<Handler>(h)});
      return data->IDCounter++;
    }

  protected:  
    Event(const Event &) = default;
    Event &operator=(const Event &) = default;

  public:
    
    struct Observer:public lars::Observer::Base{
      std::weak_ptr<Data> data;
      HandlerID id;
      
      Observer(){}
      Observer(const std::weak_ptr<Data> &_data, HandlerID _id):data(_data), id(_id){}
      
      Observer(Observer &&other) = default;
      Observer(const Observer &other) = delete;
      
      Observer & operator=(const Observer &other) = delete;
      Observer & operator=(Observer &&other)=default;
      
      void observe(const Event &event, const Handler &handler){
        reset();
        *this = event.createObserver(handler);
      }

      void reset(){
        if(auto d = data.lock()){ 
          std::lock_guard<std::mutex> lock(d->observerMutex);
          auto it = std::find_if(d->observers.begin(), d->observers.end(), [&](auto &o){ return o.id == id; });
          if (it != d->observers.end()) { d->observers.erase(it); }
        }
        data.reset();
      }
      
      ~Observer(){ reset(); }
    };
    
    Event():data(std::make_shared<Data>()){
    }

    Event(Event &&other):Event(){ *this = std::move(other); }
    Event &operator=(Event &&other){
      std::swap(data, other.data);
      return *this;
    }

    void emit(Args ... args) const {
      std::vector<std::weak_ptr<Handler>> handlers;
      handlers.resize(data->observers.size());
      data->observerMutex.lock();
      std::transform(data->observers.begin(), data->observers.end(), handlers.begin(), [](auto &h){
        return h.callback;
      });
      data->observerMutex.unlock();
      for(auto &weakCallback: handlers){
        if(auto callback = weakCallback.lock()){
          (*callback)(args...);
        }
      }
    }
    
    Observer createObserver(const Handler &h)const{
      return Observer(data, addHandler(h));
    }
    
    void connect(const Handler &h)const{
      addHandler(h);
    }
    
    void clearObservers(){
      std::lock_guard<std::mutex> lock(data->observerMutex);
      data->observers.clear();
    }
    
    size_t observerCount() const {
      std::lock_guard<std::mutex> lock(data->observerMutex);
      return data->observers.size();
    }

  };

  template <typename ... Args> class EventReference: public Event<Args...> {
  protected:
    using Base = Event<Args...>;
  
  public:
    EventReference(const Base &other):Base(other){}

    EventReference &operator=(const Base &other){
      Base::operator=(other);
      return *this;
    }
  };

  
}
