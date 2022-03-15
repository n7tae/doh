<?php
include "functions.php";
                        echo '<table cellpadding="1" border="1" style="font-family: monospace">', "\n";
                        echo '<tr><td style="text-align:center">Callsign</td><td style="text-align:center">Time Slot</td><td style="text-align:center">Talk Group</td><td style="text-align:center">Stats</td><td style="text-align:center">When</td></td>', "\n";
                        $dbname = $cfgdir.'/doh.db';
                        $db = new SQLite3($dbname, SQLITE3_OPEN_READONLY);
                        //               0       1      2       3       4
                        $ss = 'SELECT callsign,slot,talkgroup,status,strftime("%s","now")-lasttime FROM LHEARD ORDER BY 5 LIMIT '.$cfg['Dashboard']['LastHeardLength'].' ';
                        if ($stmnt = $db->prepare($ss)) {
                                if ($result = $stmnt->execute()) {
                                        while ($row = $result->FetchArray(SQLITE3_NUM)) {
                                                echo '<tr><td style="text-align:center">', CallsignToQrz($row[0]), '</td><td style="text-align:center">', $row[1], '</td><td style="text-align:center">', $row[2], '</td><td style="text-align:right">', str_replace(' ', '&nbsp;', $row[3]), '</td><td style="text-align:right">', str_replace(' ', '&nbsp;', SecToString($row[4])), '</td></tr>', "\n";
                                        }
                                        $result->finalize();
                                }
                                $stmnt->close();
                        }
                        $db->Close();
?>
