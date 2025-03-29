/**
* @file math.h
* @brief offers some mathematical functionality 

* @date 29-03.2025
* @author Jakob Wandel
* @version 1.0
**/


#pragma once


/**
 * @brief Calculates the mean of elements in a container.
 * 
 * This function works with any container that supports iteration, such as `std::vector`, `std::list`, or `std::array`.
 * It will automatically cast integral types (e.g., int) to `float`, calculate the mean, then cast it back to the original type if necessary.
 * The function is `constexpr` if possible for compile-time evaluation.
 *
 * @tparam T The container type (e.g., std::vector<int>, std::list<float>).
 * @param container The container holding the values.
 * @return The mean of the values in the container.
 */
template <typename T>
auto mean(const T& container) -> typename T::value_type {
    // Check if the container is empty
    if (container.empty()) {
        throw std::invalid_argument("Container is empty");
    }

    using ValueType = typename T::value_type;

    // If the value type is integral, cast to float for calculation and cast back later
    if constexpr (std::is_integral<ValueType>::value) {
        float sum = 0.0f;
        for (const auto& val : container) {
            sum += static_cast<float>(val);
        }
        return static_cast<ValueType>(sum / container.size());
    } else {
        // If it's not integral, simply calculate the mean
        ValueType sum = std::accumulate(container.begin(), container.end(), ValueType{0});
        return sum / container.size();
    }
}
