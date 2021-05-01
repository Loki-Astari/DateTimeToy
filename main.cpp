#include <cstdint>
#include <iostream>
#include <algorithm>


class Date
{
    static constexpr std::int64_t  normalYearCount              = 365;
    static constexpr std::int64_t  leapYearCount                = 366;
    static constexpr std::int64_t  fourYearCount                = leapYearCount + 3 * normalYearCount;
    static constexpr std::int64_t  NoramalOneHundreadYearCount  = fourYearCount * 25 - 1;
    static constexpr std::int64_t  FirstOneHundreadYearCount    = fourYearCount * 25;
    static constexpr std::int64_t  fourHundredYearCount         = FirstOneHundreadYearCount + 3 * NoramalOneHundreadYearCount;

    // Epoch Jan 1 1970 (Day 0)
    static constexpr int           greatBoundry                 = 2000;
    static constexpr std::int64_t  dayCountToGreatBoundry       = 30 * 365 + 7 /* Leap Years*/;

    static constexpr int monthCount[2][12]    = { {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
                                                  {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};

    std::int64_t        daysSinceEpoch;
    mutable int         year;
    mutable int         month;
    mutable int         day;
    mutable bool        current;
    public:
        Date(int year, int month, int day)
            : year(year)
            , month(month)
            , day(day)
            , current(true)
        {
            calculateDaysSinceEpoch();
        }
        Date& increment(int days = 1)
        {
            current = false;
            daysSinceEpoch += days;
            return *this;
        }
        Date& decrement(int days = 1)
        {
            current = false;
            daysSinceEpoch -= days;
            return *this;
        }
        friend std::ostream& operator<<(std::ostream& str, Date const& d)
        {
            d.print(str);
            return str;
        }
        void print(std::ostream& str) const
        {
            if (!current) {
                std::int64_t    daysLeft    = daysSinceEpoch;
                int             extraYear;

                // Move to the last leap year great boundary.
                // This was 1600 or 2000 Lets use 2000 (arbitrary choice)
                year = greatBoundry;
                daysLeft -= dayCountToGreatBoundry;

                // Now compensate for negative dates.
                if (daysLeft < 0) {
                    extraYear   = (-daysLeft) / fourHundredYearCount + 1;
                    year        -= extraYear * 400;
                    daysLeft    += extraYear * fourHundredYearCount;
                }

                // We should now have a positive number of number of days.
                // We should also be on a 400 hundred year boundary.

                // Remove all the 4 hundred year counts.
                extraYear   = daysLeft / fourHundredYearCount;
                year        += extraYear * 400;
                daysLeft    -= extraYear * fourHundredYearCount;

                // We should now be less than 399 years of the date.
                bool leapYearHundred = false;
                if (daysLeft <= FirstOneHundreadYearCount) {
                    leapYearHundred= true;
                }
                else {
                    year     += 100;
                    daysLeft -= FirstOneHundreadYearCount;

                    extraYear   = daysLeft / NoramalOneHundreadYearCount;
                    year        += extraYear * 100;
                    daysLeft    -= extraYear * NoramalOneHundreadYearCount;
                }

                // We are now withing 100 years of the date.

                // If we are not at the four hundred year boundary.
                // Then the first year (of the next four) is not a leap year.
                // We need to compensate for this.
                bool firstFourYearsOfNonLeapYearCentuary = false;
                if (!leapYearHundred) {

                    extraYear   = std::min(4, static_cast<int>(daysLeft / normalYearCount));
                    firstFourYearsOfNonLeapYearCentuary = extraYear != 4;

                    year        += extraYear * 1;
                    daysLeft    -= extraYear * normalYearCount;
                }

                // Remove a bunch of 4 years groups
                // The first year is always a leap year the other years are not
                extraYear   = daysLeft / fourYearCount;
                year        += extraYear * 4;
                daysLeft    -= extraYear * fourYearCount;

                // Should now be within 4 years of date.

                bool leapYear = false;
                if (daysLeft <= leapYearCount) {
                    leapYear = firstFourYearsOfNonLeapYearCentuary ? false : true;
                }
                else {
                    year        += 1;
                    daysLeft    -= leapYearCount;

                    extraYear   = daysLeft / normalYearCount;
                    year        += extraYear * 1;
                    daysLeft    -= extraYear * normalYearCount;
                }

                month = 0;
                while (daysLeft > monthCount[leapYear][month]) {
                    daysLeft    -= monthCount[leapYear][month];
                    ++month;
                }
                month += 1;
                day     = daysLeft + 1;
                current = true;
            }

            str << year << "/" << month << "/" << day << "\n";
        }
    private:
        void calculateDaysSinceEpoch()
        {
            // Epoch is Jan 1 1970
            // But we want to calculate from a great boundary
            int          diffYear;
            int          calcYear       = greatBoundry;
            daysSinceEpoch              = dayCountToGreatBoundry;

            if (year < calcYear) {
                diffYear        = (calcYear - year) / 400 + 1;
                calcYear        -= diffYear * 400;
                daysSinceEpoch  -= diffYear * fourHundredYearCount;
            }


            diffYear        = (year - calcYear) / 400;
            calcYear        += diffYear * 400;
            daysSinceEpoch  += diffYear * fourHundredYearCount;

            bool leapYearHundred = false;
            diffYear        = (year - calcYear) / 100;
            if (diffYear == 0) {
                leapYearHundred = true;
            }
            else {
                calcYear        += 100;
                daysSinceEpoch  += FirstOneHundreadYearCount;

                diffYear        -= 1;
                calcYear        += diffYear * 100;
                daysSinceEpoch  += diffYear * NoramalOneHundreadYearCount;
            }

            bool firstFourYearsOfNonLeapYearCentuary = false;
            if (!leapYearHundred) {
                diffYear = std::min(4, (year - calcYear));
                firstFourYearsOfNonLeapYearCentuary = diffYear != 4;

                calcYear        += diffYear * 1;
                daysSinceEpoch  += diffYear * normalYearCount;
            }

            diffYear        = (year - calcYear) / 4;
            calcYear        += diffYear * 4;
            daysSinceEpoch  += diffYear * fourYearCount;

            diffYear        = year - calcYear;
            bool leapYear   = false;
            if (diffYear == 0) {
                leapYear = firstFourYearsOfNonLeapYearCentuary ? false : true;
            }
            else {
                calcYear        += 1;
                daysSinceEpoch  += leapYearCount;

                diffYear        -= 1;
                calcYear        += diffYear;
                daysSinceEpoch  += diffYear * normalYearCount;
            }

            for(int loop = 0; loop < (month - 1); ++loop) {
                daysSinceEpoch  += monthCount[leapYear][loop];
            }
            daysSinceEpoch+= day - 1;
        }
};

int main()
{
    Date    d(1970, 1, 1);

    std::cout << d << "\n";

    d.increment(18705);
    std::cout << d << "\n";
}

