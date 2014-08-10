template<typename T>
using base_crange = base_range<const T>;

template<typename T>
using double_crange = double_range<const T>;

template<typename T, size_t off, size_t step>
using static_crange = static_range<const T, off, step>;

template<typename T, size_t off>
using crange_consecutive_at = range_consecutive_at<const T, off>;

template<typename T>
using crange_consecutive = range_consecutive<const T>;

template<typename T, size_t step>
using crange = range<const T, step>;

template<typename T, size_t... Is>
constexpr auto make_crange(const T* start, const T* end,
                           std::index_sequence<Is...> seq) noexcept {
    return make_range<const T, Is...>(start, end, seq);
}

template<typename T, size_t step>
using crange_osub = range_osub<const T, step>;

template<typename T, size_t off, size_t step>
using csrange_osub = srange_osub<const T, off, step>;

template<typename T, size_t... Is>
constexpr auto make_crange_osub(const T* start, const T* end,
                                std::index_sequence<Is...> seq) noexcept {
    return make_range_osub(start, end, seq);
}

template<typename T, size_t step>
using crange_sub = range_sub<const T, step>;

template<typename T, typename U, size_t step>
using crange_rect = range_rect<const T, const U, step>;

template<typename T, typename U, size_t step>
crange_rect<T, U, step> cross(crange<T, step> fst, crange<U, step> snd) {
    return crange_rect<T, U, step>(fst.curr, fst.end, snd.curr, snd.end);
}

template<typename T, size_t off, size_t step>
using csrange_sub_seq = srange_sub_seq<const T, off, step>;

template<typename T, size_t step>
using crange_sub_seq = range_sub_seq<const T, step>;

template<typename T, size_t ranges>
using cranger = ranger<const T, ranges>;