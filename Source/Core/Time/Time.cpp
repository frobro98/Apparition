// Time.cpp

//---------------------------------------------------------------------------
// HEADER FILES:
//---------------------------------------------------------------------------
#include "Time.h"
#include "Debugging/Assertion.hpp"
#include <limits>   // For numeric_limits< Time::Representation >.

//---------------------------------------------------------------------------
// FRIENDS:
//---------------------------------------------------------------------------
const Time operator*(float ratio, const Time& rhs)
{
	return(	Time(static_cast<Time::Representation>((f64)ratio * rhs.privRawTime)) );
}

const Time operator*(i32 ratio, const Time& rhs)
{
	return(Time(ratio * rhs.privRawTime));
}

const Time operator*(u32 ratio, const Time& rhs)
{
	return(Time(ratio * rhs.privRawTime));
}

//---------------------------------------------------------------------------
// CONSTRUCTORS:
//---------------------------------------------------------------------------

Time::Time(const Duration duration) : privRawTime(0)
{
	// IMPORTANT: This is private information that is SUBJECT TO CHANGE!
	//
	// Currently: 1 second = 30000
	//            1 ms     = 30
	//            1 NTSC   = 500
	//            1 PAL    = 600
	//
	// At 32 bits, this gives us a range of roughly -20 to 20 hours.
	const Time::Representation ONE_RAW_SECOND = 30000;

	switch (duration)
	{
	case ZERO:
		this->privRawTime = 0;
		break;

	case NTSC_FRAME:
		this->privRawTime = ONE_RAW_SECOND / 60;
		break;

	case NTSC_30_FRAME:
		this->privRawTime = 2 * ONE_RAW_SECOND / 60;
		break;

	case NTSC_24_FRAME:
		privRawTime = ONE_RAW_SECOND / 24;
		break;

	case PAL_FRAME:
		this->privRawTime = ONE_RAW_SECOND / 50;
		break;

	case ONE_SECOND:
		this->privRawTime = ONE_RAW_SECOND;
		break;

	case ONE_MILLISECOND:
		this->privRawTime = ONE_RAW_SECOND / 1000;
		break;

	case ONE_MINUTE:
		this->privRawTime = 60 * ONE_RAW_SECOND;
		break;

	case ONE_HOUR:
		this->privRawTime = 60 * 60 * ONE_RAW_SECOND;
		break;

	case MIN:
		this->privRawTime = std::numeric_limits< Time::Representation >::min();
		break;

	case MAX:
		this->privRawTime = std::numeric_limits< Time::Representation >::max();
		break;

	case DWORD:
		Assert(false);
		this->privRawTime = 0;
		break;

	default:
		Assert(false);
		this->privRawTime = 0;
		break;
	}
}

//---------------------------------------------------------------------------
// COMPARISONS:
//---------------------------------------------------------------------------
bool Time::operator==(const Time& rhs) const
{
	return(this->privRawTime == rhs.privRawTime);
}

bool Time::operator!=(const Time& rhs) const
{
	return(this->privRawTime != rhs.privRawTime);
}

bool Time::operator<(const Time& rhs) const
{
	return(this->privRawTime < rhs.privRawTime);
}

bool Time::operator<=(const Time& rhs) const
{
	return(this->privRawTime <= rhs.privRawTime);
}

bool Time::operator>(const Time& rhs) const
{
	return(this->privRawTime > rhs.privRawTime);
}

bool Time::operator>=(const Time& rhs) const
{
	return(this->privRawTime >= rhs.privRawTime);
}

//---------------------------------------------------------------------------
// NEGATION / ADDITION / SUBTRACTION:
//---------------------------------------------------------------------------
const Time Time::operator-() const
{
	return(Time(-this->privRawTime));
}

const Time Time::operator+(const Time& rhs) const
{
	// !!! FIXME: Overflow checks
	return(Time(this->privRawTime + rhs.privRawTime));
}

const Time Time::operator-(const Time& rhs) const
{
	// !!! FIXME: Overflow checks
	return(Time(this->privRawTime - rhs.privRawTime));
}

Time& Time::operator+=(const Time& rhs)
{
	// !!! FIXME: Overflow checks
	this->privRawTime += rhs.privRawTime;
	return(*this);
}

Time& Time::operator-=(const Time& rhs)
{
	// !!! FIXME: Overflow checks
	this->privRawTime -= rhs.privRawTime;
	return(*this);
}

//---------------------------------------------------------------------------
// MULTIPLICATION:
//---------------------------------------------------------------------------
const Time Time::operator*(float ratio) const
{
	// !!! FIXME: Overflow checks
	return(	Time(static_cast<Representation>((f64)ratio * this->privRawTime)) );
}

const Time Time::operator*(i32 ratio) const
{
	// !!! FIXME: Overflow checks
	return(Time(this->privRawTime * ratio));
}

const Time Time::operator*(u32 ratio) const
{
	// !!! FIXME: Overflow checks
	return(Time(this->privRawTime * ratio));
}

Time& Time::operator*=(float ratio)
{
	// !!! FIXME: Overflow checks
	this->privRawTime = static_cast<Representation>((f64)ratio * this->privRawTime);
	return(*this);
}

Time& Time::operator*=(i32 ratio)
{
	// !!! FIXME: Overflow checks
	this->privRawTime *= ratio;
	return(*this);
}

Time& Time::operator*=(u32 ratio)
{
	// !!! FIXME: Overflow checks
	this->privRawTime *= ratio;
	return(*this);
}


//---------------------------------------------------------------------------
// DIVISION:
//---------------------------------------------------------------------------
float Time::operator/(const Time& denominator) const
{
	// !!! FIXME: Divide by zero.
	return(	static_cast<float>(this->privRawTime) / denominator.privRawTime	);
}


const Time Time::operator/(const float denominator) const
{
	// !!! FIXME: Divide by zero.
	return(	Time(static_cast<Representation>(this->privRawTime / denominator))	);
}

const Time Time::operator/(const int denominator) const
{
	Assert(denominator != 0.0f);
	return(Time(this->privRawTime / denominator));
}

Time& Time::operator/=(const float denominator)
{
	Assert(denominator != 0.0f);
	this->privRawTime = (Representation)(this->privRawTime / denominator);
	return(*this);
}

Time& Time::operator/=(const int denominator)
{
	Assert(denominator != 0.0f);
	this->privRawTime /= denominator;
	return(*this);
}

//---------------------------------------------------------------------------
// DIVISION:
//---------------------------------------------------------------------------
int 	Time::Quotient(const Time& numerator, const Time& denominator)
{
	// !!! FIXME: Divide by zero & check range
	Assert(denominator.privRawTime != 0.0f);
	return(static_cast<int> (numerator.privRawTime / denominator.privRawTime));
}

const Time 	Time::Remainder(const Time& numerator, const Time& denominator)
{
	return(Time(numerator.privRawTime % denominator.privRawTime));
}

//---------------------------------------------------------------------------
// PRIVATE IMPLEMENTATION:
//---------------------------------------------------------------------------
Time::Time(const Time::Representation rawTime) : privRawTime(rawTime)
{
}

