#!/usr/bin/php
<?php
function p($a) { print_r($a); }
function e($s) { echo $s ."\n"; }

// get files
// in roxlu/src/roxlu: find -type f . -name "*.h" >> list.txt
$files = file_get_contents("list.txt");
$lines = explode("\n", $files);

// group by category
foreach($lines as $line) {
	$parts = explode('/', $line);
	$c = count($parts);
	array_shift($parts);
		
	$cat = array_shift($parts);
	$grouped[$cat][] = implode('/',$parts);
}
foreach($grouped as $cat => $files) {
	sort($files);
	$grouped[$cat] = $files;
	if(empty($cat)) {
		continue;
	}
	
	// create header.
	//$header_file = $cat .'/' .$cat .'.h';
//	$content = '';
	foreach($files as $file) {
		$content .= '#include "' .$cat .'/' .$file .'"' ."\n";
	}
	//file_put_contents($header_file, $content);
	
	//e($content);
	
}
e($content);
file_put_contents('Roxlu.h', $content);


//p($grouped);
	

