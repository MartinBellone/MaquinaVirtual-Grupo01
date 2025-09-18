; [0] guarda v[0]
; ...
; EFX guarda el n
; EEX guarda el contador
; EBX guarda el i (EEX * 4 + DS)

            MOV EEX, 0

; Leo la lista 1
INICIO:     MOV EBX, EEX
            MUL EBX, 4
            ADD EBX, DS
            MOV EDX, EBX
            LDL ECX, 1
            LDH ECX, 4
            MOV EAX, 0x01
            SYS 1
            CMP [EDX], 0
            JZ SIGUE
            ADD EEX, 1
            JMP INICIO

; Pongo EFX en 1 ya que por ejemplo si EFX es 2
; significa que tengo que leer un solo elemento
; en la lista 2, por eso haria EFX + 1 (Despues 
; de leer un valor) y eso me da EFX = 2.
; Entonces salgo del ciclo
SIGUE:      MOV EFX, 1

; En este momento EEX tiene n
; Ahora leo los valores de la lista 2
; usando EBX para guardarlos despues de la 
; lista 1 (n * 4 + DS)

; Si EEX es 1, no hay lista 2
            CMP EEX, 1
            JZ FIN

CICLO:      MOV EBX, EEX
            MUL EBX, 4
            ADD EBX, DS
            MOV EDX, EBX
            LDL ECX, 1
            LDH ECX, 4
            MOV EAX, 0x01
            SYS 1
            ADD EFX, 1
            CMP EEX, EFX
            JZ FIN

; Aca tengo EEX guardando n
; [0] a [n - 1] la primera lista
; [n] a [2n - 1] la segunda lista

; Ahora tendria que recorrer la lista 1 y cada valor
; buscarlo en la lista 2 y sino lo tengo es porque ese
; es el que falta
