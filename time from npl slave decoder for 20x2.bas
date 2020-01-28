#picaxe 20x2
#no_table
#terminal off
#no_end
symbol loopcounter = b55
symbol bitsvar = b0
symbol datavar = b1
symbol paritypass = bit16
symbol servar = b3
symbol yearnob = b4
symbol daynob1 = b5
symbol daynob2 = b6
symbol timenob1 = b7
symbol timenob2 = b8
symbol hour = b9
symbol minute = b10
symbol year = b11
symbol month = b12
symbol day = b13
symbol datavar2 = b14
symbol bcdtest = b15
symbol serpin = pinB.6
	
	setfreq m8
	hi2csetup i2cslave,%10110000
main:
	do
		do : loop until serpin = 1
		pause 150
		bit24 = serpin
		pause 100
		bit25 = serpin
		pause 100
		bit26 = serpin
		pause 100
		bit27 = serpin
		pause 100
		bit28 = serpin
		pause 100
		bit29 = serpin
		pause 100
		bit30 = serpin
		pause 100
		bit31 = serpin
		if servar = %00001111 then getdata
	loop

getdata:
	for ptr = 1 to 59
		do : loop until serpin = 1
		pause 150
		bit24 = serpin
		pause 100
		bit25 = serpin
		pause 100
		bit26 = serpin
		pause 100
		bit27 = serpin
		pause 100
		bit28 = serpin
		pause 100
		bit29 = serpin
		pause 100
		bit30 = serpin
		pause 100
		bit31 = serpin
		@ptr = servar
	next
	for ptr = 17 to 24
		bitsvar = @ptr
		datavar = datavar << 1 or bit0
	next
	year = datavar	
	yearnob = nob datavar
	
	datavar = 0
	for ptr = 25 to 29
		bitsvar = @ptr
		datavar = datavar << 1 or bit0
	next
	daynob1 = nob datavar
	month = datavar
	
	datavar = 0
	for ptr = 30 to 35
		bitsvar = @ptr
		datavar = datavar << 1 or bit0
	next
	daynob2 = nob datavar
	day = datavar
	
	datavar = 0
	for ptr = 39 to 44
		bitsvar = @ptr
		datavar = datavar << 1 or bit0
	next
	timenob1 = nob datavar
	hour = datavar
	
	datavar = 0
	for ptr = 45 to 51
		bitsvar = @ptr
		datavar = datavar << 1 or bit0
	next
	timenob2 = nob datavar
	minute = datavar
	
	datavar = 0
	for ptr = 52 to 59
		bitsvar = @ptr
		datavar = datavar << 1 or bit1
		datavar2 = datavar2 << 1 or bit0
	next
	paritypass = 1
	servar = yearnob + bit13
	if bit24 = 0 then : paritypass = 0 : end if
	servar = daynob1 + daynob2 + bit12
	if bit24 = 0 then : paritypass = 0 : end if
	servar = timenob1 + timenob2 + bit10
	if bit24 = 0 then : paritypass = 0 : end if
	if datavar2 <> %01111110 then : paritypass = 0 : end if
	if year > $99 then : paritypass = 0 : end if
	if month > $12 then : paritypass = 0 : end if
	if day > $31 then : paritypass = 0 : end if
	if hour > $23 then : paritypass = 0 : end if
	if minute > $59 then : paritypass = 0 : end if
	
	bcdtest = year & $0f
	if bcdtest > 9 then : paritypass = 0 : end if
	bcdtest = year & $f0 >> 4
	if bcdtest > 9 then : paritypass = 0 : end if
	bcdtest = month & $0f
	if bcdtest > 9 then : paritypass = 0 : end if
	bcdtest = month & $f0 >> 4
	if bcdtest > 9 then : paritypass = 0 : end if
	bcdtest = day & $0f
	if bcdtest > 9 then : paritypass = 0 : end if
	bcdtest = day & $f0 >> 4
	if bcdtest > 9 then : paritypass = 0 : end if
	bcdtest = hour & $0f
	if bcdtest > 9 then : paritypass = 0 : end if
	bcdtest = hour & $f0 >> 4
	if bcdtest > 9 then : paritypass = 0 : end if
	bcdtest = minute & $0f
	if bcdtest > 9 then : paritypass = 0 : end if
	bcdtest = minute & $f0 >> 4
	if bcdtest > 9 then : paritypass = 0 : end if
	
	datavar = 0
	datavar2 = 0
	
	
	ptr = 65
	@ptrinc = year
	@ptrinc = month
	@ptrinc = day
	@ptrinc = hour
	@ptr = minute
	ptr = 64
	@ptr = paritypass
	ptr = 0
	goto main