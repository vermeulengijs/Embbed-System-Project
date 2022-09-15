= Titel van het project: Game of Life =

== door:  Vermeulen Gijs ==

== Korte beschrijving ==
Conway's game of life soms wel life genoemd is een cellulaire automaat.

Deze bevind zich in een 2 dimensionaal raster met vierkante "cellen" die levend of dood kunnen zijn
en die zich volgens vastgelegde regels ontwikkelen en daarbij patronen kunnen vormen.

Dit algoritme volgt 3 hoofd regels:
	Als een levende cel door 2 of 3 levende buurcellen omgeven wordt blijft deze cel leven
	Als een levende cel door 4 of meer levende buurcellen omgeven wordt gaat deze cel dood
	Als een dode cel door precies 3 gekleurde buurcellen omgeven wordt gaat deze cel leven

Dit algoritme heb ik in labview gemaakt.	
Het labview programma gaat als server werken en het embedded systeem gaat hier via TCP/IP data van halen en weergeven op zijn LCD.
De client kan ook zelf dingen naar de server sturen.
	- via knoppen kunnen een aantal vaste patronen verstuurd worden
	- via de "eigen "knop" kan men iets in de server applicatie tekenen of als text string ingeven. 
Oneindig veel plezier !
== Libs, compiler en apps ==
 
 - lwip version LwIP V2.0.3 version ( de versie van uit het labo bestand )
 - CubeMX version 4.27.0
 - System workbench (SW4STM32) FW_F7_V1.15.0
 - Labview 2013

== Bestanden aangemaakt/aangepast ==

   Aangepaste bestanden:
	- LCD_LwIP  ( gestart van het bestand op canvas )
   Aangemaakte bestanden:
	- Labview programma ( server )
	- alle afbeeldingen gebruikt in het project ( paint ) 

== Opmerkingen == 
start het labview programma genaamd programma !
voor het labview programma te starten :
					- kan op zich werken :
						- zie dat communicatie uit staat 
						- zie dat milliseconds to wait minstens op 500 staat
						- begin het programma door via het keuze menu data in de array te sturen.  ( de keuzes :init, next , niks en communicatie versturen geen data in de array !)
						- ( hij zet data altijd in het midden van de array behalve als je de "eigen" keuze gebruikt dan kan je zelf kiezen waar je data laat komen via de eigen array )	
						- LET THE GAME BEGIN !
					- als server :
						- zie dat communicatie aan staat ( knop )
						- zie dat milliseconds to wait op 0 staat
						- zie dat grote op 20 ( server verstuurd 1 info pakket en 1 data pakket )tot 30 staat ( server verstuurd 1 info pakket en 2 data pakketten ) 
						( als we te groot gaan kan LWIP de data niet aan en verliezen we pakketten of worden pakketten random genegeerd )
						- zet de client nu aan ( hij connecteerd beter als je voor op start te klikken eerst is op stop klikt )
						- op de client kan je terwijl het programma bezig is data in de array sturen via de knoppen.
						- De server gaat maar 1 keer rekening houden met de data dat een knop verstuurd ookal blijft de knop actief ( de knop op de LCD licht groen op)
						- je kan maar 1 knop actief hebben tegelijkertijd
						- ( door nog eens op de knop te duwen desactiveer je de knop )
						- soms als je te laat drukt zal er nog een fase tussen zitten ( hangt af van de timing wanneer je klikt en wanneer de pakketten verstuurd worden)
						- LET THE FUN BEGIN

De server loopt altijd 1 fase voor op de client.
( als de client zegt trying to connect kan je best eens op stop drukken en dan terug op start drukken , ik heb ondervonden dat hij dan direct binnen raakt )

== Bronvermeldingen == 
https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life

== Optioneel: Extra's ==


