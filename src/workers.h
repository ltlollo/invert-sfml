#ifndef WORKERS_H
#define WORKERS_H

#include <vector>
#include <future>

#include "ranges.h"

namespace work {

namespace sched {

struct sequencial {};
struct roundrobin {};

}   // sched namespace

constexpr size_t nthreads{3};

template<typename T> using data_t = std::pair<T*, T*>;
template<typename T> using cdata_t = data_t<const T>;
using range::indexes;


template<typename T, typename U, typename S, size_t nworkers>
static inline void iterate(std::vector<U>& res,
                           const std::vector<T>& work_queue,
                           const size_t id,
                           const size_t load,
                           std::function<U(T)> fun,
                           sched::roundrobin) noexcept {
    for (size_t i{0}, sched{id}; i < range::split_equally(load, id, nworkers);
         ++i, sched+=nworkers) {
        res.emplace_back(fun(work_queue[sched]));
    }
}

template<typename T, typename U, typename S, size_t nworkers>
static inline void iterate(std::vector<U>& res,
                           const std::vector<T>& work_queue,
                           const size_t id,
                           const size_t load,
                           std::function<U(T)> fun,
                           sched::sequencial) noexcept {

    const size_t past_load{range::past_load(load, id, nworkers)};
    const size_t worker_load{range::split_equally(load, id, nworkers)};

    for (size_t i{past_load}; i < past_load+worker_load; ++i) {
        res.emplace_back(fun(work_queue[i]));
    }
}

template<typename T, typename U, typename S, size_t nworkers>
std::vector<U> worker(const std::vector<T>& work_queue,
                      std::function<U(T)> fun, size_t id, S sched) {
    static_assert(nworkers > 0, "need at least one worker");

    const size_t total_load{work_queue.size()};
    const size_t worker_load{range::split_equally(total_load, id, nworkers)};
    std::vector<U> res;
    if (!worker_load) {
        return res;
    }
    res.reserve(worker_load);
    iterate<T, U, S, nworkers>(res, work_queue, id, total_load, fun, sched);
    return res;
}

template<typename T, typename U, typename S = sched::sequencial,
         size_t nworkers = nthreads>
std::vector<U> work_balancer(const std::vector<T>& data,
                             std::function<U(T)>& fun,
                             S sched = sched::sequencial()) {
    std::vector<U> res;
    if(data.empty()) {
        return res;
    }
    res.reserve(data.size());
    std::array<std::future<std::vector<U>>, nworkers> workers;

    for(size_t i{0}; i < nworkers; ++i) {
        workers[i] = std::async(std::launch::async, [&data, fun, i, sched]() {
            return worker<T, U, S, nworkers>(data, fun, i, sched);
        });
    }
    for (auto& w : workers) {
        auto wres = w.get();
        res.insert(end(res), std::make_move_iterator(begin(wres)),
                   std::make_move_iterator(end(wres)));
    }
    return res;
}

template<typename R, typename T, typename U, size_t id, size_t nworkers>
std::vector<U> worker_onrange_byref(R& data, std::function<U(data_t<T>)> fun) {
    static_assert(nworkers > 0, "need at least one worker");
    static_assert(id >= 0 && id < nworkers, "not enough workers");

    std::vector<U> res;

    while(data.valid()) {
        res.emplace_back(fun(data.get()));
        data.next();
    }
    return res;
}

template<typename R, typename T, typename U, size_t id, size_t nworkers>
constexpr auto async_worker_onrange_byref(R& data, std::function<U(data_t<T>)
                                          >& fun) {
    return std::async(std::launch::async, [&data, fun]() {
        return worker_onrange_byref<R, T, U, id, nworkers>(data, fun);
    });
}

template<typename R, typename T, typename U, size_t... Is>
constexpr auto workers_onrange(std::array<R, sizeof...(Is)>& data,
                               std::function<U(data_t<T>)>& fun,
                               std::index_sequence<Is...>) {
    constexpr auto nworkers = sizeof...(Is);
    return std::array<std::future<std::vector<U>>, nworkers>{
        {async_worker_onrange_byref<R, T, U, Is, nworkers>(data[Is], fun)...}
    };
}

template<typename R, typename T, typename U, size_t id, size_t nworkers>
std::vector<U> worker_onrange(R data, std::function<U(data_t<T>)> fun) {
    static_assert(nworkers > 0, "need at least one worker");
    static_assert(id >= 0 && id < nworkers, "not enough workers");

    std::vector<U> res;

    while(data.valid()) {
        res.emplace_back(fun(data.get()));
        data.next();
    }
    return res;
}

template<typename R, typename T, typename U, size_t id, size_t nworkers>
constexpr auto async_worker_onrange(R data, std::function<U(data_t<T>)>& fun) {
    return std::async(std::launch::async, [data, fun]() {
        return worker_onrange<R, T, U, id, nworkers>(data, fun);
    });
}

template<typename T, typename U, size_t... Is>
constexpr auto csworkers_onrange_sub_seq(const std::vector<T>& data,
                                         std::function<U(cdata_t<T>)>& fun,
                                         std::index_sequence<Is...>) {
    constexpr auto nworkers = sizeof...(Is);
    return std::array<std::future<std::vector<U>>, nworkers>{
        {
            async_worker_onrange<range::csrange_sub_seq<T, Is, nworkers>,
                    const T, U, Is, nworkers>(range::csrange_sub_seq<
                                              T, Is, nworkers>(&data[0],
                                              &data[0]+data.size()), fun)...
        }
    };
}

template<typename T, typename U, size_t nworkers = nthreads>
std::vector<U> swork_balancer_sub_seq(const std::vector<T>& data,
                                      std::function<U(cdata_t<T>)>& fun) {
    std::vector<U> res;
    if(data.empty()) {
        return res;
    }
    res.reserve(data.size());
    auto workers = csworkers_onrange_sub_seq(data, fun, indexes<nworkers>());

    for (size_t i{0}; i < nworkers; ++i) {
        auto wres = workers[i].get();
        res.insert(end(res), std::make_move_iterator(begin(wres)),
                   std::make_move_iterator(end(wres)));
    }
    return res;
}

template<typename T, typename U, size_t nworkers = nthreads>
std::vector<U> work_balancer_sub_seq(const std::vector<T>& data,
                                     std::function<U(cdata_t<T>)>& fun) {
    std::vector<U> res;
    if(data.empty()) {
        return res;
    }
    auto ranges = range::cranger<T, nworkers>(&data[0],data.size())
            .get_sub_seq();
    auto workers = workers_onrange(ranges, fun, indexes<nworkers>());
    for (size_t i{0}; i < nworkers; ++i) {
        auto wres = workers[i].get();
        res.insert(end(res), std::make_move_iterator(begin(wres)),
                   std::make_move_iterator(end(wres)));
    }
    return res;
}

template<typename T, typename U, size_t nworkers = nthreads>
std::vector<U> work_balancer_osub(const std::vector<T>& data,
                                  std::function<U(cdata_t<T>)>& fun) {
    std::vector<U> res;
    if(data.empty()) {
        return res;
    }
    auto ranges = range::cranger<T, nworkers>(&data[0], data.size())
            .get_osub();
    auto workers = workers_onrange(ranges, fun, indexes<nworkers>());
    for (size_t i{0}; i < nworkers; ++i) {
        auto wres = workers[i].get();
        res.insert(end(res), std::make_move_iterator(begin(wres)),
                   std::make_move_iterator(end(wres)));
    }
    return res;
}

template<typename T, typename U, typename S, size_t id, size_t nworkers>
static inline void iterate(std::vector<U>& res,
                           const std::vector<T>& work_queue,
                           const size_t load,
                           std::function<U(T)> fun,
                           sched::roundrobin) noexcept {
    for (size_t i{0}, sched{id}; i < range::split_equally(load, id, nworkers);
         ++i, sched+=nworkers) {
        res.emplace_back(fun(work_queue[sched]));
    }
}

template<typename T, typename U, typename S, size_t id, size_t nworkers>
static inline void iterate(std::vector<U>& res,
                           const std::vector<T>& work_queue,
                           const size_t load,
                           std::function<U(T)> fun,
                           sched::sequencial) noexcept {
    const size_t past_load{range::past_load(load, id, nworkers)};
    const size_t worker_load{range::split_equally(load, id, nworkers)};

    for (size_t i{past_load}; i < past_load+worker_load; ++i) {
        res.emplace_back(fun(work_queue[i]));
    }
}

template<typename T, typename U, typename S, size_t id, size_t nworkers>
std::vector<U> static_worker(const std::vector<T>& work_queue,
                             std::function<U(T)> fun, S sched) {
    static_assert(nworkers > 0, "need at least one worker");
    static_assert(id >= 0 && id < nworkers, "not enough workers");

    const size_t total_load{work_queue.size()};
    const size_t worker_load{range::split_equally(total_load, id, nworkers)};
    std::vector<U> res;
    if (!worker_load) {
        return res;
    }
    res.reserve(worker_load);
    iterate<T, U, S, id, nworkers>(res, work_queue, total_load, fun, sched);

    return res;
}

template<typename T, typename U, typename S, size_t id, size_t nworkers>
constexpr auto async_static_worker(const std::vector<T>& data,
                                   std::function<U(T)>& fun, S sched) {
    return std::async(std::launch::async, [&data, fun, sched]() {
        return static_worker<T, U, S, id, nworkers>(data, fun, sched);
    });
}

template<typename T, typename U, typename S, size_t... Is>
constexpr auto static_workers(const std::vector<T>& data,
                              std::function<U(T)>& fun, S sched,
                              std::index_sequence<Is...>) {
    constexpr auto nworkers = sizeof...(Is);
    return std::array<std::future<std::vector<U>>, nworkers>{
        {async_static_worker<T, U, S, Is, nworkers>(data, fun, sched)...}
    };
}

template<typename T, typename U, typename S = sched::sequencial,
         size_t nworkers = nthreads>
std::vector<U> static_work_balancer(const std::vector<T>& data,
                                    std::function<U(T)>& fun,
                                    S sched = sched::sequencial()) {
    std::vector<U> res;
    if(data.empty()) {
        return res;
    }
    res.reserve(data.size());
    auto workers = static_workers(data, fun, sched, indexes<nworkers>());

    for (size_t i{0}; i < nworkers; ++i) {
        auto wres = workers[i].get();
        res.insert(end(res), std::make_move_iterator(begin(wres)),
                   std::make_move_iterator(end(wres)));
    }
    return res;
}

template<typename T>
constexpr const T* first(cdata_t<T> t) {
    return std::get<0>(t);
}

template<typename T>
constexpr const T* second(cdata_t<T> t) {
    return std::get<1>(t);
}

template<typename T>
constexpr T* first(data_t<T> t) {
    return std::get<0>(t);
}

template<typename T>
constexpr T* second(data_t<T> t) {
    return std::get<1>(t);
}

}   // work namespace

#endif // WORKERS_H
