set asmascii ""

set instable [list\
"???"\
"SAB"\
"ODU"\
"MNO"\
"DEL"\
"SAF"\
"ODF"\
"MUA"\
"AUM"\
"NUA"\
"PZA"\
"NES"\
"NUS"\
"BES"\
"PIR"\
"PPR"\
"HLT"\
]

while {[gets stdin asmascii] != -1} {
	switch -regexp $asmascii {
		^[A-Z]+.* {
			set insline [split $asmascii]
			#puts [lindex $insline 0]
			puts -nonewline stdout [binary format I [expr {int(
				[lsearch $instable [lindex $insline 0]] * 0x1000000 +
				[lindex $insline 1])
				}]]		
		}
		^[0-9]+.* {
			puts -nonewline stdout [binary format I [expr {int($asmascii)}]]
		}
	}
}