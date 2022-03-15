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
                                if (!empty($value)) {
                                        if ('"' == $value[0])
                                                list ( $value ) = explode('"', substr($value, 1));
                                        else
                                                list ( $value ) = explode(' ', $value);
                                        $value = trim($value);
                                }
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
