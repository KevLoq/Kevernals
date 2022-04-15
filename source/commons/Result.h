#pragma once

// Qt's moc is not able to parse it and triggers an error
// outcome.hp(1528): Parse error at "outcome_v2_4"
#ifndef Q_MOC_RUN
#    include "outcome.hpp"
#endif


// OC - Note: implicit default construction is disabled for result<T>. If we want to use signal/slot, the type must has
// a default constructor. It is possible to inherit from outcome::result and create a adam::Result that has a default
// constructor.

#define DEFAULT_RESULT


#ifdef DEFAULT_RESULT
// We should be able to use an anonymous namespace to hide the details of the implementation but it leads to
// warning C5046 'Symbol involving type with internal linkage not defined' with Visual Studio 15.8
// Therefore we name it "detail" instead.
namespace detail
{
// Default construct result<T> with outcome::success()
template<class T>
struct [[nodiscard]] DefaultResult : public OUTCOME_V2_NAMESPACE::result<T>
{
    using base = OUTCOME_V2_NAMESPACE::result<T>;
    using base::base;
    constexpr DefaultResult()
      : base{ OUTCOME_V2_NAMESPACE::success() }
    {
    }
};
}    // namespace detail

template<class T>
using Result = detail::DefaultResult<T>;
#else
template<class T>
using Result = OUTCOME_V2_NAMESPACE::result<T>;
#endif    // end of DEFAULT_RESULT

// Define alias to OUTCOME_V2_NAMESPACE so we can easily access outcome::success()
namespace outcome = OUTCOME_V2_NAMESPACE;
//const auto & success = static_cast<OUTCOME_V2_NAMESPACE::success_type<void>(*)()>(OUTCOME_V2_NAMESPACE::success);

