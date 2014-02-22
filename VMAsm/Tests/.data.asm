start: 	// start of program
	//ldi		r0, 128
	//syscall	SetGlobalMemory
	
	call func
	
	ret
	
func:
	xor		r0, r0, r0
	xor		r1, r1, r1
	ldi		r2, 8
	memcpy	l r0, g r1, r2
	ret
	
.DATA
	last_key	dword 0xabcdef12
	test_label
	string_size dword 1234
	string		byte[128] 0
	//test_str	byte "Hello world!", 0	
	