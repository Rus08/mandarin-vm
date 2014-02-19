start:
	ldi	r1, 16384
	add	r0, r1, 0
	syscall	SetGlobalMemory
	cmp	r0, r1
	jeq	success
	ret
success:
	xor	r0, r0, r0
	add	r2, r2, 1
cycle:
	add	r4, r2, r3
	store	r4, r0
	add	r0, r0, 4
	add	r2, r4, 0
	add	r4, r2, r3
	store	r4, r0
	add	r0, r0, 4
	add	r3, r4, 0
	cmp	r0, r1
	jls	cycle
	ret
