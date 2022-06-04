
;************************************************
;*						*
;*	Initial state 				*
;*						*
;*                      J.Savage, UNAM          *
;*						*
;*                      1/5/20                  *
;*                                              *
;************************************************




(deffacts Initial-state-objects-rooms-zones-actors


; Objects definitions
	( item (type Objects) (name deposit)(room bedroom)(image table)( attributes no-pick brown)(pose 6.183334 7.000000 0.0))
	( item (type Objects) (name storage)(room livingroom)(image table)( attributes no-pick brown)(pose 3.183334 2.000000 0.0))
	( item (type Objects) (name blockA)(room corridor)(zone deposit)(image blockA)(attributes pick)(pose 0.15 1.15 0.0))
	( item (type Objects) (name blockB)(room corridor)(zone deposit)(image blockB)(attributes pick)(pose 0.15 1.05 0.0))
	( item (type Objects) (name blockC)(room corridor)(zone deposit)(image blockC)(attributes pick)(pose 0.15 0.95 0.0))
	( item (type Objects) (name blockD)(room corridor)(zone deposit)(image blockD)(attributes pick)(pose 0.35 1.15 0.0))
	( item (type Objects) (name blockE)(room corridor)(zone deposit)(image blockE)(attributes pick)(pose 0.35 1.05 0.0))
	( item (type Objects) (name blockF)(room corridor)(zone deposit)(image blockF)(attributes pick)(pose 0.35 0.95 0.0))
	( item (type Objects) (name freespace)(room any)(zone any)(image none)(attributes none)(pose 0.0 0.0 0.0))

; Rooms definitions
	( Room (name livingroom)(zone storage)(zones dummy1 frontexit frontentrance storage dummy2)(center 0.50 0.80))
	( Room (name kitchen)(zone deposit)(zones dummy1 frontexit frontentrance deposit dummy2)(center 0.45 0.20))
	( Room (name bedroom)(zone deposit)(zones dummy1 frontexit frontentrance deposit dummy2)(center 0.4 0.10))
	( Room (name corridor)(zone deposit)(zones dummy1 frontexit frontentrance deposit dummy2)(center 0.4 0.10))
	( Room (name service)(zone deposit)(zones dummy1 frontexit frontentrance deposit dummy2)(center 0.4 0.10))
	( Room (name studio)(zone deposit)(zones dummy1 frontexit frontentrance deposit dummy2)(center 0.4 0.10))

; Robots definitions
	( item (type Robot) (name robot)(zone frontexit)(pose 1.048340 1.107002 0.0))

; Furniture definitions
	( item (type Furniture) (name cubestable)(zone bedroom)(image table)( attributes no-pick brown)(pose 6.183334 7.000000 0.0))

; Doors definitions
	( item (type Door) (name outsidedoor) (status closed) )

	( Arm (name left))

;stacks definitions
        (stack corridor deposit blockA blockB blockC)
        (stack corridor deposit blockD blockE blockF)

        (real-stack corridor deposit blockB blockA blockC)
        (real-stack corridor deposit blockD blockE blockF)


	;(goal-stack 1 corridor deposit blockD blockA blockF)
	(goal-stack 2 service deposit blockD blockE blockF)	
	(goal-stack 1 kitchen deposit blockB blockA blockC)
	;(goal-stack 2 livingroom storage blockB blockE blockC)

        (plan (name cubes) (number 0)(duration 0))

)



