<?php

/* Firebird connection info */
define('DBNAME', 'localhost:rs');
define('USER', 'rs');
define('PASS', 'rs');
define('CHARSET', 'ISO8859_1');

$SECONDS_IN_DAY = 86400;

function ColorToRGB( $_str )
{
	$red = ( $_str & 16711680 ) >> 16;
	$green = ( $_str & 65280 ) >> 8;
	$blue = $_str & 255;

	return "$red, $green, $blue";
}

function getDataset( $sql, $params = array() )
{
	static $dbh;

	// Test for interbase support
	if ( strlen( IBASE_DEFAULT ))
	{
		if ( ! isset( $dbh ) )
		{
			if ( $dbh = ibase_connect( DBNAME, USER, PASS, CHARSET ))
				echo "<!-- Connected -->\n";
			else
			{
				die("Unable to connect to server.");
			}
		}

		$stmt = ibase_prepare( $dbh, $sql );

		$s = '';
		foreach( $params as $key => $value )
		{
			$s .= '$params[\'' . $key . '\'],';
		}
		
		$s = rtrim( $s, ',' );

		if ( strlen( $s )) 
			$s =  '$rows=ibase_execute($stmt,' . $s . ');';
		else
			$s =  '$rows=ibase_execute($stmt);';

		eval( $s );

		$result = array();

		while ( $row = ibase_fetch_assoc( $rows ) )
			$result[] = $row;

		return $result;
	}
	else // try PDO - this is *NOT* the preferred way since there are some glitches. i.e. forst row fetched always comes nulled
	{
		try
		{
			if ( ! isset( $dbh ) )
			{
				if ( $dbh = new PDO('firebird:dbname=' . DBNAME, USER, PASS ))
					echo "<!-- Connected -->\n";
				else
				{
					die("Unable to connect to server.");
				}
			}
	
			$stmt =  $dbh->prepare( $sql );
	
			$i = 0;
			foreach( $params as $param )
			{
				++$i;
				$stmt->bindParam( $i, $param );
			}
	
			$stmt->execute();
	
			$result = $stmt->fetchAll( PDO::FETCH_ASSOC );
	
			$stmt = null;
				
			return $result;
		}
		catch ( PDOException $e ) 
		{
			print "Error!: " . $e->getMessage() . "<br/>";
			die();
		}
	}
}

/* Search Parameters */
if ( isset( $_GET['start'] ))
	$startDate = strtotime( $_GET['start'] );
else
	$startDate = strtotime( "last Monday" );

if ( isset( $_GET['end'] ))
{
	$endDate = strtotime( $_GET['end'] );
	$dayInterval = ( $endDate - $startDate ) / $SECONDS_IN_DAY;
}
else
{
	$endDate = $startDate + ( $SECONDS_IN_DAY * 6 );
	$dayInterval = 7;
}

$areas = getDataset('Select 
			CAST(SA.AREAID AS VARCHAR(10)) AREAID,
			CAST(A.AREA AS VARCHAR(80)) AREA,
			count(*) "COUNT"
		From 
			RESERVAS R join SALAS_AREAS SA on
				R.SALAID = SA.SALAID
			join AREAS A on
				SA.AREAID = A.AREAID
		Group by
			SA.AREAID,
			A.AREA
		Order By
			3 desc' );

if ( isset( $_GET['area'] ) )
	$areaID = $_GET['area'];
else
	$areaID = $areas[0]['AREAID'];

$salas = getDataset('Select Distinct
				CAST(SL.SALAID AS VARCHAR(10)) SALAID, 
				CAST(SL.ANDAR AS VARCHAR(10)) ANDAR, 
				CAST(SL.NOME AS VARCHAR(80)) NOME, 
				(Select First 1 
					AR.AREA 
				From 
					AREAS AR 
						Join SALAS_AREAS SA on 
							SA.AREAID = AR.AREAID 
				Where 
					SA.SALAID = SL.SALAID) AREA 
			From 
				SALAS SL 
					Join SALAS_AREAS SA on 
						SA.SALAID = SL.SALAID 
			Where 
				(? = -1) OR 
				(SA.AREAID = ?) 
			Order By 
				SL.ANDAR, SL.SALAID' , array( $areaID, $areaID ));


$reservas = getDataset('Select 
				RESERVAID, SALAID,
				CAST(DATA AS VARCHAR(10)) DATA,
				CAST(HORAIN AS VARCHAR(15)) HORAIN,
				CAST(HORAFIM AS VARCHAR(15)) HORAFIM,
				USUARIOID, USUARIO_NOME, ASSUNTO, DEPTO, NOTAS, 
				TIPO, CS.BACKGROUND, CS.FONT
			From 
				GET_RESERVAS_SEMANA(?, ?, ?) GR Left Join 
					COLOR_SCHEME CS on 
						GR.SCHEMEID = CS.SCHEMEID
			Order By 
				SALAID, 
				DATA, 
				HORAIN', array( date( 'm/d/Y', $startDate), date( 'm/d/Y', $endDate), $areaID ));

$grid = array();

// Reindex the array so it is sorted by SALAID and then DATA
foreach( $reservas as $reserva )
{
	$SALAID = $reserva['SALAID'];
	$DATA = $reserva['DATA'];
	$grid[$SALAID][$DATA][] = $reserva;
}
$reservas = null;

?>

<html>
<body>

<table style="text-align: left; width: 100%;" border="0" cellpadding="1" cellspacing="1">
	<tr>
		<td style="text-align: left; vertical-align: middle; width: 100%;">
			<form name="areaForm">
			<select onchange="location = '<? echo $_SERVER['SCRIPT_NAME']; ?>?start=<?
					echo date( 'Y-m-d', $startDate );
					?>&area=' + window.document.areaForm.Area.options[selectedIndex].value;" name="Area">
				<?
					foreach( $areas as $area )
					{
						if ( $area['AREAID'] == $areaID )
							$selected = 'selected';
						else
							$selected = '';

						echo '<option value="' . $area['AREAID'] . "\" $selected>" . $area['AREA'] . "</option>\n";
					}
				?>
			</select>
			</form>
		</td>
		<td style="text-align: center; vertical-align: middle;">
			<big>
			<? echo '<a href="' . 
				$_SERVER['SCRIPT_NAME'] . "?area=$areaID&start=" .
					date( 'Y-m-d', $startDate - ( $SECONDS_IN_DAY * 7 )) .
					'"><small><span style="font-weight: bold;">&lt;&lt;</span></small></a>' ?>
			</big>
		</td>
		<td style="text-align: right;">
			<big><big><span style="font-weight: bold;">
				<? echo date( 'd/m/Y', $startDate); ?>
			</span></big></big>
		</td>
		<td style="text-align: center; vertical-align: middle;">
			<big>
			<? echo '<a href="' . 
				$_SERVER['SCRIPT_NAME'] . "?area=$areaID&start=" .
					date( 'Y-m-d', $startDate + ( $SECONDS_IN_DAY * 7 )) .
					'"><small><span style="font-weight: bold;">&gt;&gt;<span></small></a>' ?>
			</big>
		</td>
	</tr>
</table>

<br>

<table style="text-align: left; width: 100%; height: 90%;" border="1" cellpadding="0" cellspacing="0">
<?php
	echo "\t<tr>\n";

	$cellWidth = round( 90 / ( count( $salas ) ));

	echo "\t\t<th style=\"width: 10%; background-color: rgb(225, 225, 225);\"></th>\n";

	foreach( $salas as $sala )
	{
		echo "\t\t<th style=\"text-align: center; width: $cellWidth%; background-color: rgb(225, 225, 225);\">";
		echo $sala['NOME'];
		echo "\t\t</th>";
	}

	echo "\t</tr>\n";

	for( $i = 0; $i < $dayInterval; ++$i )
	{
		$rowRawDate = $startDate + ( $i * $SECONDS_IN_DAY );
		$rowDate = date( 'Y-m-d', $rowRawDate );
		
		echo "\t<tr>\n";

		echo "\t\t<td style=\"text-align: center; vertical-align: middle; background-color: rgb(225, 225, 225);\">\n";
		echo date( 'D d', $startDate + ( $i * $SECONDS_IN_DAY ));
		echo "\t\t</td>\n";

		foreach( $salas as $sala )
		{
			$salaId = $sala['SALAID'];

			echo "\t\t<td>\n";

			if ( isset( $grid[$salaId][$rowDate] ) )
			{
				echo "\t\t\t" . '<table style="text-align: left; width: 100%;" border="1" cellpadding="0" cellspacing="0">'. "\n";
				
	
				foreach( $grid[$salaId][$rowDate] as $reserva )
				{
					echo "\t\t\t\t<tr><td>\n";
					
					/*
					$toolTip = "Resp.: " . $reserva['USUARIO'];
					if ( isset( $reserva['NOTAS'] )
						$toolTip .= '<br>' . $reserva['NOTAS'];
					*/
					
					$text = $reserva['ASSUNTO'];

					if ( isset( $reserva['DEPTO'] ) && strlen( trim($reserva['DEPTO'] )))
					{
						$text .= '<br><i>(' . $reserva['DEPTO'] . ')</i>';
					}

					$secs = ( substr( $reserva['HORAIN'], 0, 2 ) * 60 * 60 ) +
						( substr( $reserva['HORAIN'], 3, 2 ) * 60 );

					if (( $rowRawDate + $secs ) < time() )
					{
						$background = '225,225,225';
						$font = '0,0,0';
					}
					else
					{
						$background = ColorToRGB( $reserva['BACKGROUND'] );
						$font = ColorToRGB( $reserva['FONT'] );
					}

					echo '<table style="text-align: left; width: 100%; background-color: rgb(' . $background . ');" border="0" cellpadding="0" cellspacing="1"><tr><td style=" width: 10%;"><span style="color: rgb(' . $font . '); font-weight: bold;">';

					echo substr( $reserva['HORAIN'], 0, 5 );

					echo '</span></td><td colspan="1" rowspan="2" style="text-align: center;"><span style="color: rgb(' . $font . ');">';
					echo $text;
					echo '</td></tr><tr><td><span style="color: rgb(' . $font . '); font-weight: bold;">';

					echo substr( $reserva['HORAFIM'], 0, 5 );

					echo '</span></td></tr></table>';

					echo "\t\t\t\t</td></tr>\n";
				}
	
				echo "\t\t\t</table>\n";
			}
			else
				echo "\t\t\t&nbsp;\n";

			echo "\t\t</td>\n";
		}

		echo "\t</tr>\n";
	}
?>

</table>
</body>
</html>