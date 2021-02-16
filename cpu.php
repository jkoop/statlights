<?php

$new = [
	'total' => 0,
	'idle'  => 0
];

function discardNotNum($element){
	if(is_numeric($element)){
		return $element;
	}else{
		return 0;
	}
}

while(true){
	$stat = file_get_contents('/proc/stat');
	$old = $new;
	$new = [
		'total' => array_sum(array_map("discardNotNum", explode(' ', explode("\n", $stat)[0]))),
		'idle'  => explode(' ', explode("\n", $stat)[0])[5]
	];
	$diff = [
		'total' => $new['total'] - $old['total'],
		'idle'  => $new['idle']  - $old['idle']
	];
	echo ($diff['total'] - $diff['idle']) / $diff['total'] . "\n";
	sleep(1);
}

?>
