set f [open "srctest.vmt" r]

# memory map identifier -> address (offset)
set malist [list]

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
	#puts $xo
	return $xo
}

proc rpneval {temp malist rpnexpr} {
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
				lappend vrstack $token
			}
			[0-9] {
				lappend asmops $token
				lappend vrstack $token				
			}
		}
	}
	
	#puts $malist
	return $asmops
}

proc tokenize {statement malistp oplist} {
	upvar $malistp ma
	
	set t ""
	set texpr ""
	set lhs ""
	set rhs ""
	
	set t $statement
	set texpr [split $t "="]
    set lhs [regexp -all -inline {[a-z]+} [lindex $texpr 0]]
	set rhs [lindex $texpr 1]
	
	# assuming "- 1" equals "not found" or "error"
	if {[lsearch $ma $lhs] < 0} {
		# only if not already in, otherwise we get redundant mem allocs
		# 	(not a problem in itself, except for performance),
		#	HOWEVER, TODO: literal ("parameter")
		#	allocation is not mapped to identifiers (see next switch case)
		#	thus, $malist is not the same as actual allocation  
		lappend ma $lhs
	}
	
	foreach oto [rpneval $lhs $ma [dijkstrasy $rhs $oplist]] {
		puts $oto
	}	 
}

while {1} {
	set statement [gets $f]
	tokenize $statement malist $oplist
	#puts $malist
	if {[eof $f]} break
}

puts "HLT 0"

close $f