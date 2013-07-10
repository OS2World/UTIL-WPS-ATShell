:userdoc.

.* ****************************************************************************
:h1 x=left y=bottom width=100% height=100% res=001 id=intro.Die AT Desktop Shell
:p.Die AT Desktop Shell (ATSHELL) is ein einfacher RUNWORKPLACE-Ersatz.  Sie
erlaubt es eine ausf�hrbare Datei als Desktop-Programm zu starten anstatt der
Arbeitsoberfl�che.  Wird dieses Programm beendet, erscheint der ATSHELL-Dialog
erneut und erm�glicht den Neustart des Desktop-Programms, das Konfigurieren eines neuen Programms, das �ffnen
einer Befehlszeile oder den Systemabschlu�.

:p.Ein Beispiel f�r den praktischen Einsatz k�nnte z.B. die Einrichtung eines
Kiosk-Systems sein, welches nur einen Webbrowser ausf�hrt anstatt des kompletten Desktops.

:p.Man kann 2 verschiedene verschiedene Programme mit ATSHELL konfigurieren&colon.
:dl break=all.
:dt.Das Desktop-Programm
:dd.Dies ist das Programm, das als 'desktop' gestartet wird.  Vorgabe ist
PMSHELL.EXE, welches die Workplace Shell startet.
:dt.Ein Startprogramm (optional)
:dd.Man kann auch ein Programm beim Systemstart ausf�hren, bevor der ATSHELL
Dialog zum ersten Mal erscheint.  Dies ist ein Alternative zur STARTUP.CMD, mit Ausnahme
das es synchron zur Desktop-Initialisirung ausgef�hrt wird.  In anderen Worten, der
ATSHELL-Dialog erscheint nicht (und das Desktop-Programm wird nicht nicht geladen)
bis dieses Startprogramm endet, was ein potentieller Vorteil unter gewissen
Umst�nden ist.
:edl.

:p.Um die Programme zu konfigurieren, klicken Sie RECHTS auf einer leeren Zone des
ATSHELL-Dialoges, und w�hlen die entsprechende Aktion aus dem Popupmen�.  �nderungen
am Desktop-Programm treten sofort in Kraft; �nderungen am Startprogramm
werden erst beim erneuten Start der ATSHELL wirksam.

:p.Um das Desktop-Programm zu starten, w�hlen Sie den gro�en Knopf links.  Die 2
kleineren Kn�pfe erlauben es Ihnen ein Befehlszeilenfenster zu �ffnen und einen Systemabschlu�
durchzuf�hren.


.* ****************************************************************************
:h1 x=left y=bottom width=100% height=100% res=010.Using the AT Desktop Shell
:p.Dies ist das Hauptfenster der AT Desktop Shell.  Von hier aus kann man das
Desktop-Programm ausf�hren oder erh�lt Zugriff zu verschiedenen Aufgaben �ber das Popupmen� oder die
Kn�pfe.

:p.F�r eine Beschreibung der AT Desktop Shell
:link reftype=hd refid=intro.hier:elink..

.* ----------------------------------------------------------------------------
:h2 x=left y=bottom width=100% height=100% res=011.Der Programmkonfigurationsdialog
:p.Dieser Dialog erlaubt es das zu startende Programm zu konfigurieren.

:dl break=all.
:dt.Pfad- und Dateiname
:dd.Geben Sie den vollst�ndigen Name des Programms ein (oder nutzen Sie den Bl�tternknopf zur
Lokalisierung).  Dies kann eine ausf�hrbare Bin�rdatei oder eine CMD-Datei sein.
:dt.Parameter
:dd.Optional.  Weitere Parameters zur �bergabe an das Programm angeben.
:dt.Arbeitsverzeichnis
:dd.Optional.  Arbeitsverzeichnis des Programmes angeben.  Das Programm wird
in diesem aktuellen Verzeichnis ausgef�hrt.
:edl.

:euserdoc.

