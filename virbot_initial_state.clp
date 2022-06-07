
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
	( item (type Objects) (name Apple)(room corridor)(zone deposit)(image Apple)(attributes pick)(pose 0.15 1.15 0.0))
	( item (type Objects) (name Sushi)(room corridor)(zone deposit)(image Sushi)(attributes pick)(pose 0.15 1.05 0.0))
	( item (type Objects) (name Milk)(room corridor)(zone deposit)(image Milk)(attributes pick)(pose 0.15 0.95 0.0))
	( item (type Objects) (name Soap)(room corridor)(zone deposit)(image Soap)(attributes pick)(pose 0.35 1.15 0.0))
	( item (type Objects) (name Perfume)(room corridor)(zone deposit)(image Perfume)(attributes pick)(pose 0.35 1.05 0.0))
	( item (type Objects) (name Shampoo)(room corridor)(zone deposit)(image Shampoo)(attributes pick)(pose 0.35 0.95 0.0))
	( item (type Objects) (name freespace)(room any)(zone any)(image none)(attributes none)(pose 0.0 0.0 0.0))

; Rooms definitions
	( Room (name livingroom)(zone storage)(zones dummy1 frontexit frontentrance storage dummy2)(center 0.50 0.80))
	( Room (name kitchen)(zone deposit)(zones dummy1 frontexit frontentrance deposit dummy2)(center 0.45 0.20))
	( Room (name bedroom)(zone deposit)(zones dummy1 frontexit frontentrance deposit dummy2)(center 0.4 0.10))
	( Room (name corridor)(zone deposit)(zones dummy1 frontexit frontentrance deposit dummy2)(center 0.0 0.0))
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
        (stack corridor deposit Apple Sushi Milk)
        (stack corridor deposit Soap Perfume Shampoo)

        (real-stack corridor deposit Apple Sushi Milk)
        (real-stack corridor deposit Soap Perfume Shampoo)

	(goal-stack 2 service deposit Soap Perfume Shampoo)	
	(goal-stack 1 kitchen deposit Sushi Apple Milk)

        (plan (name cubes) (number 0)(duration 0))

)



