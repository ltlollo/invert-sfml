#ifndef RANGES_H
#define RANGES_H

#include <array>
#include <functional>
#include <tuple>

namespace range {

template<size_t N> using indexes = std::make_index_sequence<N>;

constexpr auto split_equally(const size_t load, const size_t id,
                             const size_t nworkers) {
    return (load % nworkers != 0 && id < load % nworkers) ?
                load/nworkers + 1 : load/nworkers;
}

template<size_t rbound, size_t start = 0, size_t step = 1>
struct range_looping {
    size_t value{start};
    auto next() noexcept { value = (value+step)%rbound; }
    auto operator++() noexcept { next(); return value; }
};

template<typename T>
struct base_range {
    void next() noexcept {
        static_cast<T*>(this)->__next();
    }
    auto get() const noexcept {
        return static_cast<const T*>(this)->__get();
    }
    auto operator++() noexcept { next(); return get(); }
    auto operator()() const noexcept { return get(); }
    bool valid() const noexcept { return get(); }
    T& operator*() const noexcept { return *get(); }
};

template<typename T>
struct double_range : base_range<T> {
    bool valid() const noexcept {
        return static_cast<const T*>(this)->__valid();
    }
};

template<typename T, size_t off, size_t step>
struct static_range : base_range<static_range<T, off, step>> {
    T* curr;
    T* end;

    static_range(T* start, T* end) noexcept :
        end{end} {
        curr= start+off < end ? start+off : nullptr;
    }
    auto __get() const noexcept { return curr; }
    void __next() noexcept {
        if (curr) {
            curr =  curr+step < end ? curr+step : nullptr;
        }
    }
};

template<typename T, size_t off>
using range_consecutive_at = static_range<T, off, 1>;

template<typename T>
using range_consecutive = range_consecutive_at<T, 0>;

template<typename T, size_t step>
struct range : base_range<range<T, step>> {
    T* curr;
    T* end;

    range(T* start,T* end, size_t off) noexcept : end{end} {
        curr = start+off < end ? start+off : nullptr;
    }
    auto __get() const noexcept { return curr; }
    void __next() noexcept {
        if (curr) {
            curr =  curr+step < end ? curr+step : nullptr;
        }
    }
};

template<typename T, size_t step>
struct range_itseq  : base_range<range_itseq<T, step>> {
    T* curr_;
    T* end_;

    range_itseq(T* start,T* end, size_t id) noexcept {
        if (end <= start) { curr_ = nullptr; return; }
        auto load = split_equally(end-start, id, step);
        if (!load) { curr_ = nullptr; return; }
        curr_ = start+(id*load);
        end_ = start+(id+1)*load;
    }
    auto __get() const noexcept { return curr_; }
    void __next() noexcept {
        if (curr_) {
            curr_ =  curr_+1 < end_ ? curr_+1 : nullptr;
        }
    }
};

template<typename T, size_t... Is>
constexpr auto make_range(T* start, T* end, std::index_sequence<Is...>)
noexcept {
    constexpr auto ranges = sizeof...(Is);
    return std::array<range<T, ranges>, ranges>{
        range<T, ranges>(start, end, Is)...
    };
}

template<typename T, size_t step>
struct range_osub : double_range<range_osub<T, step>> {
    T* scurr;
    T* fcurr;
    T* end;
    const size_t off;

    void __advance_sec() noexcept {
        scurr = scurr+step < end ? scurr+step : nullptr;
    }
    void __advance_first() noexcept {
        fcurr = fcurr+2+off < end ? fcurr+1 : nullptr;
    }

    bool __valid() const noexcept { return fcurr; }
    range_osub(T* start, T* end, const size_t off) noexcept
        : end{end}, off{off} {
        scurr = (start+off+1) < end ? start+off+1 : nullptr;
        fcurr = (start < end && scurr) ? start : nullptr;
    }
    auto __get() const noexcept {
        return fcurr ? std::make_pair(fcurr, scurr) :
                       std::make_pair<T*, T*>(nullptr, nullptr);
    }
    void __next() noexcept {
        if (!fcurr) return;
        __advance_sec();
        if (scurr) return;
        __advance_first();
        if (fcurr) scurr = fcurr+off+1;
    }
};

template<typename T, size_t off, size_t step>
struct srange_osub : double_range<srange_osub<T, off, step>> {
    T* scurr;
    T* fcurr;
    T* end;

    void __advance_sec() noexcept {
        scurr = scurr+step < end ? scurr+step : nullptr;
    }
    void __advance_first() noexcept {
        fcurr = fcurr+2+off < end ? fcurr+1 : nullptr;
    }

    bool __valid() const noexcept { return fcurr; }
    srange_osub(T* start, T* end) noexcept
        : end{end} {
        scurr = (start+off+1) < end ? start+off+1 : nullptr;
        fcurr = (start < end && scurr) ? start : nullptr;
    }
    auto __get() const noexcept {
        return fcurr ? std::make_pair(fcurr, scurr) :
                       std::make_pair<T*, T*>(nullptr, nullptr);
    }
    void __next() noexcept {
        if (!fcurr) return;
        __advance_sec();
        if (scurr) return;
        __advance_first();
        if (fcurr) scurr = fcurr+off+1;
    }
};

template<typename T, size_t... Is>
constexpr auto make_range_osub(T* start, T* end, std::index_sequence<Is...>)
noexcept {
    constexpr auto ranges = sizeof...(Is);
    return std::array<range_osub<T, ranges>, ranges>{
        range_osub<T, ranges>(start, end, Is)...
    };
}

template<typename T, size_t step>
struct range_sub : double_range<range_sub<T, step>> {
    T* fcurr;
    T* end;
    size_t off;
    range<T, step> scurr;

    void __advance_first() noexcept {
        fcurr = fcurr+2+off < end ? fcurr+1 : nullptr;
    }

    range_sub(T* start, T* end, const size_t off) noexcept
        : end{end}, off{off}, scurr(start+1, end, off) {
        fcurr = (start<end && scurr.valid()) ? start : nullptr;
    }
    auto __get() const noexcept {
        return fcurr ? std::make_pair(fcurr, scurr.curr) :
                       std::make_pair<T*, T*>(nullptr, nullptr);
    }
    void __next() noexcept {
        if (!fcurr) { return; }
        if (++scurr) { return; }
        __advance_first();
        if (fcurr) {
            scurr = range<T, step>(fcurr+1, end, off);
        }
    }
    bool __valid() const noexcept { return fcurr; }
};

template<typename T, size_t step>
struct range_sub_seq : double_range<range_sub_seq<T, step>> {
    T* fcurr;
    T* end;
    size_t off;
    range_itseq<T, step> scurr;

    void __advance_first() noexcept {
        fcurr = fcurr+2+off < end ? fcurr+1 : nullptr;
    }

    range_sub_seq(T* start, T* end, const size_t off) noexcept
        : end{end}, off{off}, scurr(start+1, end, off) {
        fcurr = (start<end && scurr.valid()) ? start : nullptr;
    }
    auto __get() const noexcept {
        return fcurr ? std::make_pair(fcurr, scurr.curr_) :
                       std::make_pair<T*, T*>(nullptr, nullptr);
    }
    void __next() noexcept {
        if (!fcurr) { return; }
        if (++scurr) { return; }
        __advance_first();
        if (fcurr) {
            scurr = range_itseq<T, step>(fcurr+1, end, off);
        }
    }
    bool __valid() const noexcept { return fcurr; }
};

template<typename T, size_t id, size_t step>
struct srange_itseq  : base_range<srange_itseq<T, id, step>> {
    T* curr_;
    T* end_;

    srange_itseq(T* start, T* end) noexcept {
        if (end <= start) { curr_ = nullptr; return; }
        auto load = split_equally(end-start, id, step);
        if (!load) { curr_ = nullptr; return; }
        curr_ = start+(id*load);
        end_ = start+(id+1)*load;
    }
    auto __get() const noexcept { return curr_; }
    void __next() noexcept {
        if (curr_) {
            curr_ =  curr_+1 < end_ ? curr_+1 : nullptr;
        }
    }
};

template<typename T, size_t off, size_t step>
struct srange_sub_seq : double_range<srange_sub_seq<T, off, step>> {
    T* fcurr;
    T* end;
    srange_itseq<T, off, step> scurr;

    void __advance_first() noexcept {
        fcurr = fcurr+2+off < end ? fcurr+1 : nullptr;
    }
    srange_sub_seq(T* start, T* end) noexcept
        : end{end}, scurr(start+1, end) {
        fcurr = (start<end && scurr.valid()) ? start : nullptr;
    }
    auto __get() const noexcept {
        return fcurr ? std::make_pair(fcurr, scurr.curr_) :
                       std::make_pair<T*, T*>(nullptr, nullptr);
    }
    void __next() noexcept {
        if (!fcurr) { return; }
        if (++scurr) { return; }
        __advance_first();
        if (fcurr) {
            scurr = srange_itseq<T, off, step>(fcurr+1, end);
        }
    }
    bool __valid() const noexcept { return fcurr; }
};

template<typename T, size_t... Is>
constexpr auto make_range_sub_seq(T* start, T* end,
                                  std::index_sequence<Is...>) noexcept {
    constexpr auto ranges = sizeof...(Is);
    return std::array<range_sub_seq<T, ranges>, ranges>{
        {range_sub_seq<T, ranges>(start, end, Is)...}
    };
}

template<typename T, size_t... Is>
constexpr auto make_crange_sub_seq(const T* start, const T* end,
                                   std::index_sequence<Is...> seq) noexcept {
    return make_range_sub_seq(start, end, seq);
}

template<typename T, typename U, size_t step>
struct range_rect : double_range<range_rect<T, U, step>> {
    range_consecutive<T> first;
    range<U, step> second;
    U* sstart;
    const size_t off;

    range_rect(T* fstart, T* fend, U* sstart, U* send,
               const size_t off) noexcept
        : sstart{sstart}, off{off}, first(fstart, fend),
          second(sstart, send, off) {
    }
    auto __get() const noexcept {
        return first.valid() ? std::make_pair(first(), second()) :
                               std::make_pair<T*, U*>(nullptr, nullptr);
    }
    void __next() noexcept {
        if (!first.valid()) { return; }
        if (++second) { return; }
        second = range<U, step>(sstart, second.end, off);
        first.next();
    }
    bool __valid() const noexcept { return first.valid(); }
};

template<typename T, typename U, size_t step>
range_rect<T, U, step> cross(range<T, step> fst, range<U, step> snd) {
    return range_rect<T, U, step>(fst.curr, fst.end, snd.curr, snd.end);
}

template<typename T, size_t ranges>
class ranger {
    T* start;
    T* end;
public:
    constexpr ranger(T* start, const size_t size)
        : start{start}, end{start+size} {
    }
    template<size_t N> ranger(T* start[N]) : start{start}, end{start+N} {
    }
    constexpr auto get() noexcept {
        return make_range(start, end, indexes<ranges>());
    }
    constexpr auto get_osub() noexcept {
        return make_range_osub(start, end, indexes<ranges>());
    }
    constexpr auto get_sub_seq() noexcept {
        return make_range_sub_seq(start, end, indexes<ranges>());
    }
};

#include "ranges_const.h"

}   // range namespace

#endif // RANGES_H
