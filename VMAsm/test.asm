start: 	// start of program
	add[1]	r0, r1, r2
	add		r1, r1, r20
	call	func
	or		r0, r0, r2
	ret


func:	// test function
	add	r0, r0, r0
	ret
	