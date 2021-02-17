<?php

$new = [
	'total' => 0,
	'idle'  => 0
];

while(true){
	$stat = file_get_contents('/proc/stat');
	$stat = explode(' ', explode("\n", $stat)[0]);
	foreach($stat as $key => $value){
		if(!is_numeric($value)){
			unset($stat[$key]);
		}
	}
	$stat = array_values($stat);

	$old = $new;
	$new = [
		'total' => array_sum($stat),
		'idle'  => $stat[3]
	];
	$diff = [
		'total' => $new['total'] - $old['total'],
		'idle'  => $new['idle']  - $old['idle']
	];

	echo '$old["total"]:  ' . $old['total'] . "\n";
	echo '$old["idle"]:   ' . $old['idle'] . "\n";
	echo '$new["total"]:  ' . $new['total'] . "\n";
	echo '$new["idle"]:   ' . $new['idle'] . "\n";
	echo '$diff["total"]: ' . $diff['total'] . "\n";
	echo '$diff["idle"]:  ' . $diff['idle'] . "\n";
	echo 'load:           ' . ($diff['total'] - $diff['idle']) / $diff['total'] . "\n\n";
	sleep(2);
}

?>
