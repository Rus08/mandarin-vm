start: 	// start of program
	ldi		r0, 128
	syscall	SetGlobalMemory
	
	xor		r0, r0, r0
	ldi		r1, OnKeyDown
	syscall	RegisterCallback
	
	xor		r0, r0, r0
	store	r0, 4
cycle:
	syscall	DispatchCallbacks
	load	r0, 0
	cmp		r0, 112 // F1 key
	jne		cycle
	
	ret


OnKeyDown:
	loadl	r2, 0
	store	r2, 0
	load	r1, 4
	add		r0, r1, 8
	storeb  r2, r0
	add		r1, r1, 1
	store	r1, 4
	ldi		r0, 8
	syscall	DebugOutput
	ret
	


	
	
	