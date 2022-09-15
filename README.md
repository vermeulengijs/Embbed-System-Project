# Embbed-System-Project

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

Het labview programma gaat als server werken en het embedded systeem (STM32F746GD) gaat hier via TCP/IP data van halen en weergeven op zijn LCD.
De client kan ook zelf dingen naar de server sturen.
	- via knoppen kunnen een aantal vaste patronen verstuurd worden
	- via de "eigen "knop" kan men iets in de server applicatie tekenen of als text string ingeven. 
Oneindig veel plezier !

== Libs, compiler en apps ==
 
 - lwip version LwIP V2.0.3 version ( de versie van uit het labo bestand )
 - CubeMX version 4.27.0
 - System workbench (SW4STM32) FW_F7_V1.15.0
 - Labview 2013
