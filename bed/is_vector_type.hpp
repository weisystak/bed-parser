#pragma once
#include <vector>

/// @class IsVectorType template class
/// @brief served as a simple type verifier indicating a to-be-tested object corresponds to std::vector types or not
/// @tparam T a type parameter
template < typename T >
struct IsVectorType 
{
/// @memberof value corresponds to the value of false by default
 	static const bool value = false;
};

/// @brief specialized form of the IsVectorType, with the to-be-tested object specialized as the type of std::vector<T>
template < typename T >
struct IsVectorType < std::vector <T> > 
{
/// @memberof value corresponds to the value of true 
 	static const bool value = true;
};
