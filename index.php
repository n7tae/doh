<!DOCTYPE html>
<?php
$cfg = array();
$fmodule = $furcall = '';
$cfgdir = '/usr/local/etc';

function IniParser(string $filepath, &$kvarray)
{
	if ($lines = file($filepath, FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES)) {
		$section = '';
		foreach ($lines as $line) {
			$line = trim($line);
			if ($line[0] == '#') continue;
			if ($line[0] == '[')
				$section = trim($line, '[]');
			else {
				if (! strpos($line, '=')) continue;
				if (0 == strlen("$section")) continue;
				list( $key, $value ) = explode('=', $line);
				if ('"' == $value[0])
					list ( $value ) = explode('"', substr($value, 1));
				else
					list ( $value ) = explode(' ', $value);
				$value = trim($value);
				$kvarray[$section][$key] = $value;
			}
		}
	}
}

function GetIP(string $type)
{
	if ('internal' == $type) {
		$iplist = explode(' ', `hostname -I`);
		foreach ($iplist as $ip) {
			if (strpos($ip, '.')) break;
		}
	} else if ('ipv6' == $type)
		$ip = trim(`dig @resolver1.ipv6-sandbox.opendns.com AAAA myip.opendns.com +short -6`);
	else if ('ipv4' == $type)
		$ip = trim(`dig @resolver4.opendns.com myip.opendns.com +short -4`);
	else
		$ip = '';
	return $ip;
}

function SecToString(int $sec) {
	if ($sec >= 86400)
		return sprintf("%0.2f days", $sec/86400);
	$hrs = intdiv($sec, 3600);
	$sec %= 3600;
	$min = intdiv($sec, 60);
	$sec %= 60;
	if ($hrs) return sprintf("%2d hr  %2d min ", $hrs, $min);
	if ($min) return sprintf("%2d min %2d sec ", $min, $sec);
	return sprintf("%d sec ", $sec);
}

function CallsignToQrz(string $my)
{
	$my = trim($my);
	if (0 == strlen($my)) {
		$my = 'No Call ';
	} else {
		if (preg_match('/^[0-9]+$/', $my)) {
			$len = strlen($my);
		} else {
			if (strpos($my, ' '))
				$link = strstr($my, ' ', true);
			else
				$link = $my;
			$len = strlen($my);
			$my = '<a target="_blank" href="https://www.qrz.com/db/'.$link.'">'.$my.'</a>';
		}
		while ($len < 8) {
			$my .= ' ';
			$len += 1;
		}
	}
	return $my;
}

IniParser($cfgdir.'/dmr.cfg', $cfg);
?>

<html>
<head>
<title>DOH Dashboard</title>
<meta http-equiv="refresh" content="<?php echo $cfg['Dashboard']['Refresh'];?>">
</head>
<body>
<h2>DOH <?php echo $cfg['General']['Callsign']; ?> Dashboard</h2>

<?php
$showlist = explode(',', $cfg['Dashboard']['ShowOrder']);
foreach($showlist as $section) {
	switch ($section) {
		case 'PS':
			if (`ps -aux | grep -e DMRGateway -e doh | wc -l` > 1) {
				echo 'Processes:<br><code>', "\n";
				echo str_replace(' ', '&nbsp;', 'USER       PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND<br>'), "\n";
				$lines = explode("\n", `ps -aux | grep -e doh -e DMRGateway | grep -v -e grep -e journal`);
				foreach ($lines as $line) {
					echo str_replace(' ', '&nbsp;', $line), "<br>\n";
				}
				echo '</code>', "\n";
			}
			break;
		case 'SY':
			echo 'System Info:<br>', "\n";
			$hn = trim(`uname -n`);
			$kn = trim(`uname -rmo`);
			$osinfo = file('/etc/os-release', FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);
			foreach ($osinfo as $line) {
				list( $key, $value ) = explode('=', $line);
				if ($key == 'PRETTY_NAME') {
					$os = trim($value, '"');
					break;
				}
			}
			$cu = trim(`cat /proc/cpuinfo | grep Model`);
			if (0 == strlen($cu))
				$cu = trim(`cat /proc/cpuinfo | grep "model name"`);
			$culist = explode("\n", $cu);
			$mnlist = explode(':', $culist[0]);
			$cu = trim($mnlist[1]);
			if (count($culist) > 1)
				$cu .= ' ' . count($culist) . ' Threads';
			if (file_exists('/opt/vc/bin/vcgencmd'))
				$cu .= ' ' . str_replace("'", '&deg;', trim(`/opt/vc/bin/vcgencmd measure_temp`));
			echo '<table cellpadding="1" border="1" style="font-family: monospace">', "\n";
			echo '<tr><td style="text-align:center">CPU</td><td style="text-align:center">Kernel</td><td style="text-align:center">OS</td><td style="text-align:center">Hostname</td></tr>', "\n";
			echo '<tr><td style="text-align:center">', $cu, '</td><td style="text-align:center">', $kn, '</td><td style="text-align:center">', $os, '</td><td style="text-align:center">', $hn, '</td></tr></table><br>', "\n";
			break;
		case 'LH':
			echo 'Last Heard:<br>', "\n";
			echo '<table cellpadding="1" border="1" style="font-family: monospace">', "\n";
			echo '<tr><td style="text-align:center">Callsign</td><td style="text-align:center">Time Slot</td><td style="text-align:center">Talk Group</td><td style="text-align:center">Stats</td><td style="text-align:center">When</td></td>', "\n";
			$dbname = $cfgdir.'/doh.db';
			$db = new SQLite3($dbname, SQLITE3_OPEN_READONLY);
			//               0       1      2       3       4
			$ss = 'SELECT callsign,slot,talkgroup,status,strftime("%s","now")-lasttime FROM LHEARD ORDER BY 5 LIMIT '.$cfg['Dashboard']['LastHeardLength'].' ';
			if ($stmnt = $db->prepare($ss)) {
				if ($result = $stmnt->execute()) {
					while ($row = $result->FetchArray(SQLITE3_NUM)) {
						echo '<tr><td style="text-align:center">', CallsignToQrz($row[0]), '</td><td style="text-align:center">', $row[1], '</td><td style="text-align:center">', $row[2], '</td><td style="text-align:right">', str_replace(' ', '&nbsp;', $row[3]), '</td><td style="text-align:right">', str_replace(' ', '&nbps;', SecToString($row[4])), '</td></tr>', "\n";
					}
					$result->finalize();
				}
				$stmnt->close();
			}
			$db->Close();
			echo '</table><br>', "\n";
			break;
		case 'IP':
			$hasv6 = stristr(GetCFGValue('ircddb0_host'), 'v6');
			if (! $hasv6) $hasv6 = stristr(GetCFGValue('ircddb1_host'), 'v6');
			echo 'IP Addresses:<br>', "\n";
			echo '<table cellpadding="1" border="1" style="font-family: monospace">', "\n";
			echo '<tr><td style="text-align:center">Internal</td><td style="text-align:center">IPV4</td>';
			if ($hasv6) echo '<td style="text-align:center">IPV6</td></tr>';
			echo "\n";
			echo '<tr><td>', GetIP('internal'), '</td><td>', GetIP('ipv4'), '</td>';
			if ($hasv6) echo '<td>', GetIP('ipv6'), '</td></tr>';
			echo "\n", '</table><br>', "\n";
			break;
		default:
			echo 'Section "', $section, '" was not found!<br>', "\n";
			break;
	}
}
?>
<br>
<p align="center">DOH Dashboard Version 210220 Copyright &copy; by Thomas A. Early, N7TAE.</p>
</body>
</html>
