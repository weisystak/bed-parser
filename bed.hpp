/// @file bed.hpp
/// @brief A parser of bed format file 

#pragma once
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/spirit/include/qi.hpp>
#include <Biovoltron/format/bed/header.hpp>
#include <Biovoltron/format/bed/is_tuple_type.hpp>
#include <Biovoltron/format/bed/is_vector_type.hpp>
#include <vector>
#include <tuple>

namespace biovoltron::format{

using BEDHeader = bed::Header;

/// @class BED
/// @brief BED class which store BED data and BED header.
///
/// Member variable<br>
///         BEDHeader& header<br>
///         TupleType data<br>   
///
/// Member functions<br>
///         to_string<br>
///         set_bed_data<br>
///         dump<br>
///         get_obj<br>
///         fill<br>
///         to_comma_delimited_string<br>
///         to_string<br>
///
/// Non-member functions<br>
///         operator>>
///         operator<<
///     
/// This class store BED data and header from an BED format file.
/// The BED class only stores one data entry from the BED file. 
///  reference.

    template<class TupleType = std::tuple <std::string, uint32_t, uint32_t> >
    class BED
    { 
      public:
        
        static_assert( IsTupleType<TupleType>::value, "ARGUMENT IS NOT A TUPLE");
        

        void set_bed_data(std::string& str)
        {
            constexpr auto tup_size = std::tuple_size<TupleType>::value;
            std::vector<std::string> split_string;
            boost::split( split_string, str, boost::is_any_of( " \t" ), boost::token_compress_on );

            fill<TupleType, tup_size>(data, split_string);
        }


        friend std::istream& operator>>(std::istream& is, BED<TupleType>& bed)
        {
            get_obj(is, bed);
            return is;
        }

        friend std::ostream& operator<<(std::ostream& os, BED<TupleType>& bed)
        {
            os << bed.to_string()<<"\n";
            return os;
        }

        
        std::string to_string()
        {
            std::string str;
            constexpr auto tup_size = std::tuple_size<TupleType>::value;
            
            this->to_string<TupleType, tup_size>(this->data, str);
            
            str.erase(str.end()-1);
            return str;
        }


        static std::istream& get_obj(std::istream& is, BED<TupleType>& bed)
        {
            std::string str;
            if(std::getline(is, str))
                bed.set_bed_data(str);

            return is;
        }

        static void dump(std::ostream& os, std::vector<BED>& v_bed)
        {
            if(v_bed.size() == 0)
                return;

            os << v_bed[0].header.to_string() << "\n";

            for(auto& bed: v_bed)
                os << bed.to_string() << "\n";
        }
        

        template<int i, class TUPLETYPE>
        using element_t = std::tuple_element_t<i, TUPLETYPE>;

        template<class TUPLETYPE, int i>
        void fill(TUPLETYPE& t, std::vector<std::string>& split_str)
        {
            if constexpr(i == 0)
                return;
            else
            {
                fill<TUPLETYPE, i-1>(t, split_str);

                if constexpr( std::is_same<char, 
                                        element_t<i-1, TUPLETYPE>
                                        >::value )
                {
                    std::get<i-1>(t) = split_str[i-1][0];
                }
                else if constexpr ( std::is_integral<element_t<i-1, TUPLETYPE>>::value)
                {
                    std::get<i-1>(t) = stoi(split_str[i-1]);
                }
                else if constexpr( std::is_same<std::string, 
                                        element_t<i-1, TUPLETYPE>
                                        >::value )
                {
                    std::get<i-1>(t) = split_str[i-1];
                }
                else if constexpr( std::is_same<std::string, 
                                        element_t<i-1, TUPLETYPE>
                                        >::value )
                {
                    std::get<i-1>(t) = split_str[i-1];
                }
                else if constexpr( IsVectorType<
                                    element_t<i-1, TUPLETYPE>
                                        >::value )
                {
                    boost::spirit::qi::phrase_parse(split_str[i-1].begin(), split_str[i-1].end(), 
                    boost::spirit::qi::int_ % ',', boost::spirit::ascii::space, std::get<i-1>(t));
                }
                else   // tuple type
                {
                    using nested_tuple = element_t<i-1, TUPLETYPE>;
                    static_assert( IsTupleType<nested_tuple>::value, "ARGUMENT IS NOT A TUPLE");
                    
                    std::vector<std::string> split_str2;
                    boost::split( split_str2, split_str[i-1], boost::is_any_of( "," ), boost::token_compress_on );
                    
                    fill<nested_tuple, std::tuple_size<nested_tuple>::value>(std::get<i-1>(t), split_str2);
                }
            }
        }

        template<class...Ts>
        void to_comma_delimited_string( std::tuple<Ts...>& t, std::string& str)
        {
            std::apply ([&str] (const auto &... elem)
                        {
                            str += ((std::to_string(elem) + ",") + ...); 
                        },
                        t);
        }

        template<class TUPLETYPE, int i>
        void to_string(TUPLETYPE& t, std::string& str)
        {
            if constexpr(i == 0)
                return;
            else
            {
                to_string<TUPLETYPE, i-1>(t, str);

                if constexpr( std::is_same<char, 
                                        element_t<i-1, TUPLETYPE>
                                        >::value )
                {
                    str += std::get<i-1>(t);
                }
                else if constexpr ( std::is_integral<element_t<i-1, TUPLETYPE>>::value)
                {
                    str += std::to_string(std::get<i-1>(t));
                }
                else if constexpr( std::is_same<std::string, 
                                        element_t<i-1, TUPLETYPE>
                                        >::value )
                {
                    str += std::get<i-1>(t);
                }
                else if constexpr( IsVectorType<
                                    element_t<i-1, TUPLETYPE>
                                        >::value )
                {
                    for(auto it = std::get<i-1>(t).begin(); it != std::get<i-1>(t).end(); it++)
                        str += std::to_string(*it) + ",";
                }
                else   // tuple type
                {
                    using nested_tuple = element_t<i-1, TUPLETYPE>;
                    //static_assert( IsTupleType<nested_tuple>::value, "ARGUMENT IS NOT A TUPLE");
                    
                    to_comma_delimited_string(std::get<i-1>(t), str);
                }
            
                str += "\t";
            }
        }

        BEDHeader header;
        TupleType data;

    };

}
