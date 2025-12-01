#ifndef EVENT_BUS_HPP
#define EVENT_BUS_HPP

template<typename EventData, int MAX_SUBSCRIBERS>
class EventBus {
public:
    typedef void (*Callback)(const EventData&);

    EventBus() : count(0) {}

    bool subscribe(Callback cb) {
        if (count >= MAX_SUBSCRIBERS) return false;
        subscribers[count++] = cb;
        return true;
    }

    void publish(const EventData& data) {
        for (int i = 0; i < count; i++) {
            if (subscribers[i]) subscribers[i](data);
        }
    }

private:
    Callback subscribers[MAX_SUBSCRIBERS];
    int count;
};

#endif
