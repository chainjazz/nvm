set f [open "srctest.vmt" r]

set oplist [list\
	[list "*" "/"]\
	[list "+" "-"]\
	[list " "]\
]

# memory allocation (on program load)
set malist [list]
# literal values (on program load)
set vllist [list]

proc dij_getoppre {op oplist} {
	for {set i 0} {$i < [llength $oplist]} {incr i} {
		set splist [lindex $oplist $i]

		for {set j 0} {$j < [llength $splist]} {incr j} {
			if {[lindex $splist $j] eq $op} {
				#puts "operator match!"
				return $i				
			}
		}
	}	
}

proc dijkstrasy {xi ol} {
	set so [list " "]
	set xo ""
	
	foreach a $xi {
		switch -regexp $a {
			[a-z0-9]+ {
				#puts "add token ($a) to output"
				append xo " $a"
			}			
			[\*\/\+\-]+ {
				set solast [lindex $so end]
				 
				while {(([dij_getoppre $a $ol] == [dij_getoppre $solast $ol]) ||
						([dij_getoppre $a $ol] > [dij_getoppre $solast $ol])) &&
						($solast ne "(")} {
					#puts "pop $solast from stack to output"
					append xo " $solast"
					#pop
					set so [lreplace $so end end]
					set solast [lindex $so end]
				}
				#puts "push token ($a) to stack"
				lappend so $a
			}			
			[\(]+ { lappend so $a }
			[\)]+ {
				set solast [lindex $so end]
				
				while { $solast ne "(" } {
					#puts "pop $solast from stack to output"
					append xo " $solast"
					#pop					
					set so [lreplace $so end end]
					set solast [lindex $so end]
				}
				
				# pop left bracket
				set so [lreplace $so end end]
			}
		}
	}
	
	set solast [lindex $so end]

	while {[llength $so] > 0} {
		append xo " $solast"
		set so [lreplace $so end end]
		set solast [lindex $so end]
	}
	
	return $xo
}

proc rpneval {temp rpnexpr} {
	set malist [list $temp]
	set asmops [list]
	set vrstack [list]
	set lplh ""
	set rplh ""
	
	foreach token $rpnexpr {
		switch -regexp $token {
			[\*\/\+\-] {
				set rplh [lsearch $malist [lindex $vrstack end]]
				set vrstack [lreplace $vrstack end end]
				set lplh [lsearch $malist [lindex $vrstack end]]
				set vrstack [lreplace $vrstack end end]
				#puts "VRN ($lplh,$rplh)"
				lappend asmops "MUA $lplh"
				if {$token eq "+"} { lappend asmops "SAB $rplh" }
				if {$token eq "-"} { lappend asmops "ODU $rplh" }
				if {$token eq "*"} { lappend asmops "MNO $rplh" }
				if {$token eq "/"} { lappend asmops "DEL $rplh" }
				lappend asmops "AUM [lsearch $malist $temp]"
				lappend vrstack "$temp"
			}
			[a-z] {
				lappend malist $token
				lappend vrstack $token
			}
			[0-9] {
				lappend asmops $token
				lappend vrstack $token
			}
		}
	}
	
	return $asmops
}

proc tokenize {t} {
	global oplist	
	set xinfx ""
	set texpr [split $t "="]
    set lhs [lindex $texpr 0]
	set rhs [lindex $texpr 1]	
	set xinfx $rhs
	
	foreach oto [rpneval $lhs [dijkstrasy $xinfx $oplist]] {
		puts $oto
	}
	
	#puts "AUM $lhs"
}

while {1} {
	set t [gets $f]
	tokenize $t
	if {[eof $f]} break
}

close $f