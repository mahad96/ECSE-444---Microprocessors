	AREA test, CODE, READONLY
	
	export asm_am		
asm_am

loop
	SUBS R3, R3, #1
	BLT done
	VLDR.f32 S0, [R0], #4
	VLDR.f32 S1, [R1], #4
	VMUL.f32 S2, S0, S1
	VSTR.f32 S2, [R2], #4
	B loop
	
done

	BX LR                         
	
	END
