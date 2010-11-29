### Begin-lib timedate
# These functions operate on absolute dates & times.
#
# Requires epochdays library.
# Conversions involving implicit (unspecified) timezone adjustments can only
# be done if running under gawk.
#
# The current version of this library can be found by searching
# http://www.armory.com/~ftp/
#
# @(#) timedate 1.8.3 2009-01-02
# 1988       john h. dubois iii (john@armory.com)
# 1988-1997  Various enhancements
# 1997-12-13 Converted to deal with both / and - as date component separators
# 1999-04-11 Added error descriptions to ParseDate
# 1999-11-14 Added minimal error checking to date2unixtime()
# 2000-01-05 1.1 Added special -1 and -2 years to date2unixtime()
#            and Nearest param to lDate2unixtime()
# 2002-02-21 1.2 date2unixtime(): Added debug param.  Clearified meaning of
#            year param, and made it work for years passed as 0-69.
# 2003-05-12 1.3 Made date2unixtime() use mktime() if a TZ adjustment is
#            requested.  Excised half-baked TZ code.
# 2004-06-03 1.4 Added Time and Timezone params to date2unixtime()
# 2004-10-16 1.5 Got rid of ParseDate(), makedate(), unmakedate()
# 2005-01-10 1.6 Added timeFormatter()
# 2005-04-20 1.7 Let AM or PM be included in time arg to date2unixtime()
# 2006-06-13 1.7.1 Added ignoreBadTimeElem to date2unixtime()
# 2007-10-31 1.8 Let -1 also be given for month & day in date2unixtime()
# 2008-07-12 1.8.1 Fixed date2unixtime to allow whitespace before meridian
# 2008-10-02 1.8.2 Fixed some current-time comparisons in date2unixtime()
# 2009-01-02 1.8.3 Fixed another current-time comparison in date2unixtime()

# todo: maybe switch to using the prefix lookup routines in eventrem?
function init_td_monthName2Num(\
	Month, Months)
{
    split("jan,feb,mar,apr,may,jun,jul,aug,sep,oct,nov,dec",Months,",")
    for (Month in Months)
	_td_monthName2Num[Months[Month]] = sprintf("%02d",Month)
}

# Convert a file timestamp as printed by "ls -l" to an epoch time.
# Example l lines:
# drwsr-xr-x  29 root     bin         2560 Sep 24 23:48 .
# -r--r--r--   1 root     root        1742 Jul  5  1995 .
# In these cases, fields 6, 7, and 8 would be passed as Month, Day, and
# YearOrTime.
# If a time is given, it may optionally include seconds.
# Month may be a month name/abbreviation or number
# 
# noTZ should be 1 if the date output was produced with a 0 TZ;
# in this case no attempt is made to undo the TZ adjustment.
# If Nearest is true and no year is given, the year is assumed to be the
#   previous, current, or next year, whichever would place the result closest
#   to the present.  This is useful for converting schedule entries & such.
# If Nearest is false and no year is given, the year is assumed to be the
#   current year unless that would place the time in the future; in that case,
#   the year is assumed to be the previous year.  This is correct for most
#   purposes, including converting the output of ls -l.
# On error, -1 is returned.
function lDate2unixtime(Month, Day, YearOrTime, noTZ, Nearest, debug,

	time, Year)
{
    # Deal with varying dates printed by l
    # Use year if given
    # Subtract 1 from year if month given is from last year
    if (index(YearOrTime,":")) {	# If year is actually time...
	Year = Nearest ? -2 : -1
	time = YearOrTime
    }
    else
	Year = YearOrTime
    return date2unixtime(Year, Month, Day, time, noTZ ? "0" : "", debug)
}

# Returns the number of seconds that passed from 1970 Jan 1 00:00:00
# to the given date.
# Timezone should be given as a numeric offset from GMT in seconds.
# Use 0 for Timezone if the date being converted was not generated with a
# timezone adjustment.
function unixtime(Year, Month, Day, Hour, Minute, Second, Timezone)
{
    return ((YMD2day(Year,Month,Day) * 24 + Hour) * 60 + Minute) * 60 + \
    Second + Timezone
}

# date2unixtime() returns the number of seconds that passed from 1970 Jan 1
# 00:00:00 GMT to the given date and time.
# Note that if the given date occurred during the fall-back period of a
# daylight savings time transition, its mapping to an epoch time is ambiguous.
#
# If Timezone is set to an empty string, the date is assumed to be in the local
#   timezone.  This can only be done if the code is being run by gawk, which
#   has mktime(), and if a date is given that occurred during the fall-back
#   period of a daylight savings time transition, its mapping to an epoch time
#   is ambiguous.
#   If Timezone is set to 0, no timezone adjustment is done.  Otherwise
#   Timezone should be a standard US timezone name or an offset from GMT in
#   RFC2822 format.
# If Year is -1, see below.
# If Year is -2, the year is assumed to be the previous, current, or next year,
#   whichever would place the result closest to the present.
# previous year.
# If Year < 70, it is taken to be in the century 2000.
# If 70 <= Year <= 1000, it is taken to be an offset from the year 1900.
# Otherwise, Year is taken to be a complete year specification.
# A year earlier than 1970 results in an error.
#
# Month can be a numeric month or a standard 3-letter month abbreviation.

# Year, Month, and Day may be given as -1.  This indicates that the current
# date's instance of that component should be used, unless it would place the
# date in the future, in which case the next-most-current instance of that
# component should be used.  If Year is not given this way, then Month may not;
# and if Month is not, then Day may not.  If Day is given this way, then a time
# must be specified.  If multiple elements are given as -1, they are filled in
# from shortest-period to longest-period time-unit, so that as a whole they
# will be set to the most recent date that does not give a time in the future.
# Note that if Year and Month are given this way, and Day is set to a day of
# the month that does not exist in the previous month (for example, 31 when the
# current month is October), and Time specifies a time after the current time
# of day, the result will not be a date in the previous month.

# If Time is given, it must be in the form hours[:minutes[:seconds]]
# with an optional meridian at the end in the form (case ignored):
# [optional-whitespace]am|pm|a.m.|p.m.
# If it is not given, the time is taken to be midnight.
#
# If ignoreBadTimeElem is true, bad time elements are set to minimal values intead
# of resulting in an error.
#
# Globals: Sets/uses MDays[], _TZmap[]
# On error, -1 is returned and _date2unixtime_err is set to a string describing
# the error.
function date2unixtime(Year, Month, Day, Time, Timezone, debug,
	ignoreBadTimeElem,

	LeapDays, curtime, yearDay, diffDOY, timeElem, numTimeElem, timeSec,
	Hours, Minutes, Seconds, meridian, curDay, curMonth, curYear, curDOY,
	compAreLater)
{
    if (debug)
	printf "date2unixtime: Got: year=%s month=%s day=%s time=%s timezone=%s\n",
	Year,Month,Day,Time,Timezone > "/dev/stderr"

    curtime = systime()

    # Process time
    numTimeElem = split(Time, timeElem, ":")
    if (match(timeElem[numTimeElem], /[[:blank:]]*[aApP]\.?[mM]\.?$/)) {
	merid = toupper(substr(timeElem[numTimeElem], RSTART, RLENGTH))
	timeElem[numTimeElem] = substr(timeElem[numTimeElem], 1, RSTART-1)
    }
    if (numTimeElem > 3 || numTimeElem > 0 && timeElem[1] !~ /^[0-9]+$/ || numTimeElem > 1 &&
	    timeElem[2] !~ /^[0-9]+$/ || numTimeElem > 2 && timeElem[3] !~ /^[0-9]+$/) {
	if (ignoreBadTimeElem)
	    split("0:0:0", timeElem, ":")
	else {
	    _date2unixtime_err = "Bad Time: " Time
	    return -1
	}
    }
    Hours = timeElem[1]+0
    if (merid != "") {
	if (Hours == 12)
	    Hours = 0
	if (index(merid, "P"))
	    Hours += 12
    }
    Minutes = timeElem[2]+0
    Seconds = timeElem[3]+0

    # Process day
    # If all date elements are set to most-recent, time must be given.
    # If day of month is set to most-recent, month must be set to most-recent.
    if (Day == -1 && (numTimeElem == 0 || Month != -1) || Day+0 < -1 || Day+0 > 31) {
	_date2unixtime_err = "Bad Day: " Day
	return -1
    }
    Day += 0	# Don't do this until Day no longer needed for error messages
    if (Day == -1 || Month == -1 || Year == -1) {
	# If any most-recent calculations will be done, keep track of whether
	# the components converted so far are later than the equivalent
	# current-time components
	compAreLater = Hours * 3600 + Minutes * 60 + Seconds > \
		strftime("%H", curtime) * 3600 + strftime("%M", curtime) * 60 \
		+ strftime("%S", curtime)
	curDay = strftime("%d")+0
	if (Day == -1) {
	    Day = curDay
	    if (compAreLater)
		Day--	# might become 0, but that's OK
	}
    }

    # Process month
    if (Month ~ /^[[:alpha:]][[:alpha:]][[:alpha:]]$/) {
	if (!("jan" in _td_monthName2Num))
	    init_td_monthName2Num()
	if (!(tolower(Month) in _td_monthName2Num)) {
	    _date2unixtime_err = "Bad Month: " Month
	    return -1
	}
	Month = _td_monthName2Num[tolower(Month)]
    }
    else if (Month == -1 && Year != -1 || Month+0 < -1 || Month+0 > 12) {
	_date2unixtime_err = "Bad Month: " Month
	return -1
    }
    Month += 0
    if (Month == -1 || Year == -1) {
	if (Day != curDay)
	    compAreLater = Day > curDay
	curMonth = strftime("%m")+0
	if (Month == -1) {
	    Month = curMonth
	    if (compAreLater && --Month == 0)
		Month = 12
	}
    }

    # Process year
    if (!MDays[2])
	split("0 31 59 90 120 151 181 212 243 273 304 334 365",MDays," ")
    # This does not take leap year into consideration, but is close enough
    # for year determination
    yearDay = MDays[Month] + Day
    if (Year+0 < 0) {
	curYear = strftime("%Y")
	if (Year+0 == -1) {
	    Year = curYear
	    if (Month != curMonth)
		compAreLater = Month > curMonth
	    if (compAreLater)
		Year--
	}
	else if (Year+0 == -2) {	# Do windowing
	    curDOY = strftime("%j",curtime)+0
	    # Use year that would place date closest to present
	    diffDOY = curDOY - yearDay
	    Year = curYear
	    if (diffDOY > 182)
		# If date is a much earlier day of year than the present,
		# it is probably from next year
		Year++
	    else if (diffDOY < -182)
		# If date is a much later day of year than the present,
		# it is probably from last year
		Year--
	}
	else {
	    _date2unixtime_err = "Bad Year: " Year
	    return -1
	}
	if (debug)
	    printf "date2unixtime: generated year: %d\n",Year > "/dev/stderr"
    }
    # Convert year to years since 1900
    Year += 0
    if (Year >= 1970)
	Year -= 1900
    else if (Year < 70)
	Year += 100
    else if (Year >= 1000) {
	_date2unixtime_err = "Bad Year: " Year
	return -1
    }

    # Year is now year since 1900
    # Month is 1-origin month
    # Day is 1-origin day of month

    LeapDays = int((Year - 68) / 4)
    if (Month <= 2 && Year % 4 == 0)
	LeapDays -= 1

    if (Timezone == "") {
	Year += 1900
	if (debug)
	    printf "date2unixtime: final: year=%d month=%d day=%d hours=%d minutes=%d seconds=%d\n",
		    Year, Month, Day, Hours, Minutes, Seconds > "/dev/stderr"
        # mktime() expects 4-digit year, 1-origin month & day, hour, minute,
        # second, and optional DST flag (which we leave at the default).
        ret = mktime(Year " " Month " " Day " " Hours " " Minutes " " Seconds)
	if (ret == -1 && ignoreBadTimeElem)
	    ret = mktime(Year " " Month " " Day " 0 0 0")
	if (ret == -1)
	    _date2unixtime_err = "mktime failed"
	return ret
    }
    else if (Timezone ~ /^[-+][0-9]?[0-9][0-9][0-9]$/) {  # allow 3-digit TZ
	if (length(Timezone) == 4)
	    Timezone = substr(Timezone, 1, 1) "0" substr(Timezone, 2)
	TZoffset = substr(Timezone,1,1) (substr(Timezone,2,2)*3600 + substr(Timezone,4,2)*60)
    }
    else if (Timezone != "0") {
	if (!("PST" in _TZmap)) {
	    _TZmap["UT"] = _TZmap["GMT"] = 0
	    _TZmap["EDT"] = -4*3600
	    _TZmap["CDT"] = _TZmap["EST"] = -5*3600
	    _TZmap["MDT"] = _TZmap["CST"] = -6*3600
	    _TZmap["PDT"] = _TZmap["MST"] = -7*3600
	    _TZmap["PST"] = -8*3600
	    _TZmap["BST"] = _TZmap["CET"] = _TZmap["MEZ"] = 3600
	}
	if (toupper(Timezone) in _TZmap)
	    TZoffset = _TZmap[toupper(Timezone)]
	else {
	    if (ignoreBadTimeElem)
		TZoffset = 0
	    else {
		_date2unixtime_err = "Bad Timezone: " Timezone
		return -1
	    }
	}
    }
    if (debug)
	printf "date2unixtime: final: year=%d yearday=%d leapdays=%d hours=%d minutes=%d seconds=%d tzoff=%d\n",
	Year,yearDay,LeapDays,Hours,Minutes,Seconds,TZoffset > "/dev/stderr"
    return ((Year - 70) * 365 + yearDay - 1 + LeapDays) * 24 * 3600 + Hours*3600 + Minutes*60 + Seconds - TZoffset
}

# Only works for current time... does *not* take a systime argument!
function my_strftime(Format,

	Time)
{
    Cmd = "date \"+" Format "\""
    Cmd | getline Time
    close(Cmd)
    return Time
}

# timeFormatter: Generate a time formatter that includes only those components
# that are meaningful for a given time precision.
# 
# Input variables:
# 
# template is the formatter template.  It has the form:
# formatter|formatter|...
# Each formatter segment should have at most one % formatter in it.
# Example: %Y|-%m|-%d| |%H|:%M|:%S
# The currently known formatters are: %Y %m %d %H %M %S
# 
# precision is the precision of the timestamps that are to be formatted, in
# seconds.
# 
# The template is split into formatters, and each formatter is included only if
# the precision is no more than 90% of the size of the next-largest common
# formatter.  For example, the next largest common formatter after %S is %M,
# so a formatter that includes %S will be included only if the precision is no
# more than 90% of 60 seconds (54 seconds).
#
# Components that do not include a formatter are always included.
# Leading and trailing whitespace are removed.

function timeFormatter(template, precision,

	numFmt, formatters, i, formatter, char, pos, period, tm, out)
{
    if (!(1 in _timeFormatter_periods)) {
	# No need for real precision here
	split("60 60 24 30 12", tm, " ")
	period = 0.9
	for (i = 1; i in tm; i++)
	    _timeFormatter_periods[i] = period *= tm[i]
    }
    numFmt = split(template, formatters, /\|/)
    for (i = 1; i <= numFmt; i++) {
	formatter = formatters[i]
	pos = index(formatter, "%")
	if (pos > 0) {
	    char = substr(formatter, pos+1, 1)
	    pos = index("SMHdmY", char)
	    if (!pos)
		return ""	# error
	    period = _timeFormatter_periods[pos]
	    if (period == "" || precision <= period)
		out = out formatter
	}
	else
	    out = out formatter
    }
    gsub(/^[[:space:]]+|[[:space:]]+$/,"",out)
    return out
}

### End-lib timedate
