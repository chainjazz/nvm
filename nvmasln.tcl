set ins 0

set addoff 0
set objin [read stdin]

binary scan $objin I* ins

foreach a $ins {
	puts -nonewline [binary format I [expr {int($a) + $addoff}]]
	if {$a == 0} {set addoff 100}
}
