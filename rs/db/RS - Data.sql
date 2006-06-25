INSERT INTO "SALAS"	 ("SALAID", 	"ANDAR", 	"NOME") VALUES (	2, 	2, 	'Sala 2');
INSERT INTO "SALAS"	 ("SALAID", 	"ANDAR", 	"NOME") VALUES (	1, 	2, 	'Sala de Guerra');
INSERT INTO "SALAS"	 ("SALAID", 	"ANDAR", 	"NOME") VALUES (	3, 	2, 	'Sala 3');
INSERT INTO "SALAS"	 ("SALAID", 	"ANDAR", 	"NOME") VALUES (	4, 	4, 	'Sala 4');

INSERT INTO "COLOR_SCHEME"	 ("SCHEMEID", 	"BACKGROUND", 	"FONT", 	"BORDER") VALUES (	0, 	15459244, 	15400960, 	0);
INSERT INTO "COLOR_SCHEME"	 ("SCHEMEID", 	"BACKGROUND", 	"FONT", 	"BORDER") VALUES (	1, 	13107142, 	1998622, 	0);
INSERT INTO "COLOR_SCHEME"	 ("SCHEMEID", 	"BACKGROUND", 	"FONT", 	"BORDER") VALUES (	2, 	16758782, 	6432845, 	0);
INSERT INTO "COLOR_SCHEME"	 ("SCHEMEID", 	"BACKGROUND", 	"FONT", 	"BORDER") VALUES (	3, 	11977983, 	2373755, 	0);
INSERT INTO "COLOR_SCHEME"	 ("SCHEMEID", 	"BACKGROUND", 	"FONT", 	"BORDER") VALUES (	4, 	16772073, 	11971072, 	0);
INSERT INTO "COLOR_SCHEME"	 ("SCHEMEID", 	"BACKGROUND", 	"FONT", 	"BORDER") VALUES (	5, 	16774046, 	7221810, 	0);
INSERT INTO "COLOR_SCHEME"	 ("SCHEMEID", 	"BACKGROUND", 	"FONT", 	"BORDER") VALUES (	6, 	11206624, 	43058, 	0);
INSERT INTO "COLOR_SCHEME"	 ("SCHEMEID", 	"BACKGROUND", 	"FONT", 	"BORDER") VALUES (	7, 	12517266, 	474880, 	0);
INSERT INTO "COLOR_SCHEME"	 ("SCHEMEID", 	"BACKGROUND", 	"FONT", 	"BORDER") VALUES (	8, 	16752546, 	15400968, 	0);
INSERT INTO "COLOR_SCHEME"	 ("SCHEMEID", 	"BACKGROUND", 	"FONT", 	"BORDER") VALUES (	9, 	15460360, 	4867848, 	0);

INSERT INTO MODELOS ("MODELOID", "NOME", "BODY", "ITEM", "FUNDO", "FUNDOALT")
VALUES (GEN_ID("GENMODELOS", 1),
'Default',
'<html>
  <body>
    <div align="center" >
      <h4>%SALA% - <i>%DATA%</i> </h4>
      <table cellspacing="0" border="1" cellpadding="2" align="center" width="90%">
        %BODY%
      </table>
    </div>
  </body>
</html>', 
'<tr bgcolor="%COLOR%">
  <td width="15%"  valign="middle" align="center">
    <table cellspacing="0" border="0" cellpadding="0"  width="100%">
      <tr>
        <td valign="middle" align="center">
          <b><big>%HORAIN%</big></b>
        </td>
      </tr>
      <tr>
        <td valign="middle" align="center">
          <b><big>%HORAFIM%</big></b>
        </td>
      </tr>
    </table>
  </td>
  <td valign="middle" align="center">
    <h2>%ASSUNTO%<br><i>%DEPTO%</i></h2>
  </td>
</tr>', 
'White', 
'#DDDDDD');

INSERT INTO MODELOS ("MODELOID", "NOME", "BODY", "ITEM", "FUNDO", "FUNDOALT")
VALUES (GEN_ID("GENMODELOS", 1),
'Compacto',
'<html>
  <body>
    <div align="center" >
      <h4>%SALA% - <i>%DATA%</i> </h4>
      <table cellspacing="0" border="1" cellpadding="2" align="center" width="90%">
        %BODY%
      </table>
    </div>
  </body>
</html>', 
'<tr bgcolor="%COLOR%">
  <td valign="middle" align="center" width="15%">
    <b>%HORAIN%</b>
  </td>
  <td valign="middle" align="center"  width="15%">
    <b>%HORAFIM%</b>
  </td>
  <td valign="middle" align="center"  width="70%">
    %ASSUNTO%&nbsp;<i>%DEPTO%</i>
  </td>
</tr>',
'White', 
'#DDDDDD');

COMMIT WORK;
