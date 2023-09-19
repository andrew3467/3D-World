//
// Created by apgra on 9/19/2023.
//

#ifndef APPLICATION_SINGLETON_H
#define APPLICATION_SINGLETON_H

template <typename T>
class Singleton
{
protected:
    Singleton(){}
    virtual ~Singleton(){;}

public:
    Singleton(const Singleton &) = delete;
    Singleton & operator=(const Singleton &) = delete;
    Singleton(Singleton &&) = delete;
    Singleton & operator=(Singleton &&) = delete;

    static T & get()
    {
        static T instance;
        return instance;
    };
};

#endif //APPLICATION_SINGLETON_H
