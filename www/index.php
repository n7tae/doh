<!DOCTYPE html>
<?php
include "functions.php";
?>

<html>
<head>
<title>DOH Dashboard</title>
</head>
<body>
<h2>DOH <?php echo $cfg['General']['Callsign']; ?> Dashboard</h2>

<?php
$showlist = explode(',', $cfg['Dashboard']['ShowOrder']);
foreach($showlist as $section) {
	switch ($section) {
		case 'PS':
			if (`ps -aux | grep -e DMRGateway -e doh | wc -l` > 1) {
				echo '<b>Processes:</b><br><code>', "\n";
				echo str_replace(' ', '&nbsp;', 'USER       PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND<br>'), "\n";
				$lines = explode("\n", `ps -aux | grep -e doh -e DMRGateway | grep -v -e grep -e journal`);
				foreach ($lines as $line) {
					echo str_replace(' ', '&nbsp;', $line), "<br>\n";
				}
				echo '</code>', "\n";
			}
			break;
		case 'SY':
			echo '<b>System Info:</b><br>', "\n";
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
			echo '</center>';
			break;
		case 'LH':
			echo '<b>Last Heard:</b><br>', "\n";
?>
			<DIV id="new">
			<SCRIPT src="jquery.min.js"></SCRIPT>
			<SCRIPT>
                	var page="lh.php";
                	var page30="none";
	                $("#new").load(page);
        	        var auto_refresh = setInterval(
            		function ()
                	{
                 		if(page != "none"){
                        	$("#new").load(page);
                        }
                 	count30++;
                 		if(count30>=30) {
                 		  if(page30 != "none") {
                    		   $("#new").load(page30);
                        }
                 		count30=0;
                 		}
                	}, 1000);
			</SCRIPT></DIV>
<?php
			echo '</table><br>', "\n";
			break;
		case 'IP':
			echo '<b>IP Addresses:</b><br>', "\n";
			echo '<table cellpadding="1" border="1" style="font-family: monospace">', "\n";
			echo '<tr><td style="text-align:center">Internal</td><td style="text-align:center">IPv4</td>';
			echo "\n";
			echo '<tr><td>', GetIP('internal'), '</td><td>', GetIP('ipv4'), '</td>';
			echo "\n", '</table><br>', "\n";
			break;
		default:
			echo 'Section "', $section, '" was not found!<br>', "\n";
			break;
	}
}
?>
<br>
<p align="center">DOH Dashboard Version 220312 Copyright &copy; by Thomas A. Early, N7TAE.</p>
</body>
</html>
