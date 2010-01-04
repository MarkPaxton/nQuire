<?php

global $user;
$node_details = get_inquiry_details_from_current_path();

$output = '';

// group's selected measures
$selected_measures = "465,586,587,588,589,460,459,461,463,464,462";

// get group data
$result = array();

foreach (explode(',', $selected_measures) as $measure_id) {

    if ($measure_id != 0) {
        $result[] = get_data_for_inquiry_measure_and_user ($node_details->inquiry_id, $measure_id, $user->uid);
    };
};

// sort group data
array_multisort(
    $result[3], SORT_ASC, SORT_NUMERIC,
    $result[2], SORT_ASC, SORT_NUMERIC,
    $result[1], SORT_ASC, SORT_NUMERIC,
    $result[4], SORT_ASC, SORT_STRING,
    $result[0], SORT_ASC, SORT_STRING
);

// Now rotate the values table so that each measure's data
// is in a column (rather than a row)
$i = 0;
$j = 0;
$flipped_results = array();

foreach ($result as $row) {
    foreach ($row as $col) {
        $flipped_results[$i][$j] = $col;
        $i = $i + 1;
    };
    $j = $j + 1;
    $i = 0;
};

// generate table headings for group data
$headings = get_selected_measures_headings_for_inquiry_and_user ($node_details->inquiry_id, $selected_measures, $user->uid);

//chart_1 = smell
//
// now make the chart
$chart_1 = array(
    '#chart_id' => 'smell_chart',
    '#title' => chart_title(t('Bananas smell'), 'cc0000', 15),
    '#type' => CHART_TYPE_BAR_V_GROUPED,
    '#size' => chart_size(940, 250),
    '#adjust_resolution' => TRUE,
    '#bar_size' => chart_bar_size(8, 2),
);

// use separate arrays for each data set (optional)
$op = array();
$ou = array();
$vp = array();
$vu = array();

$counter = 0;
foreach ($result[9] as $item) {
    $remainder = $counter % 4;
    if ($remainder == 0) {
        $op[] = $item;
    }
    elseif ($remainder == 1) {
        $ou[] = $item;
    }
    elseif ($remainder == 2) {
        $vp[] = $item;
    }
    elseif ($remainder == 3) {
        $vu[] = $item;
    };
    $counter = $counter + 1;
};

// add data sets to chart
$chart_1['#data']['op'] = $op;
$chart_1['#data']['ou'] = $ou;
$chart_1['#data']['vp'] = $vp;
$chart_1['#data']['vu'] = $vu;

// add chart legend for each data set
$chart_1['#legends'][] = 'Organic packaged';
$chart_1['#legends'][] = 'Organic unpackaged';
$chart_1['#legends'][] = 'Value packaged';
$chart_1['#legends'][] = 'Value unpackaged';

// colour each data set differently
$chart_1['#data_colors'][] = 'ff0000';
$chart_1['#data_colors'][] = '00ff00';
$chart_1['#data_colors'][] = '0000ff';
$chart_1['#data_colors'][] = 'F4C400';

// add y axis labels
$chart_1['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][0][] = chart_mixed_axis_label(t(' 00'));
$chart_1['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][0][] = chart_mixed_axis_label(t(' 01'));
$chart_1['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][0][] = chart_mixed_axis_label(t(' 02'));
$chart_1['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][0][] = chart_mixed_axis_label(t(' 03'));
$chart_1['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][0][] = chart_mixed_axis_label(t(' 04'));
$chart_1['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][0][] = chart_mixed_axis_label(t(' 05'));
$chart_1['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][0][] = chart_mixed_axis_label(t(' 06'));
$chart_1['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][0][] = chart_mixed_axis_label(t(' 07'));

$chart_1['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][1][] = chart_mixed_axis_label(t('Smell'), 90);

// add x axis labels
$chart_1['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('05 pm'));
$chart_1['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('06 am'));
$chart_1['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('06 pm'));
$chart_1['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('07 am'));
$chart_1['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('07 pm'));
$chart_1['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('08 am'));
$chart_1['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('08 pm'));
$chart_1['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('09 am'));
$chart_1['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('09 pm'));
$chart_1['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('10 am'));
$chart_1['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('10 pm'));
$chart_1['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('11 am'));
$chart_1['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('11 pm'));
$chart_1['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('12 am'));
$chart_1['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('12 pm'));
$chart_1['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('13 am'));
$chart_1['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('13 pm'));

$chart_1['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][3][] = chart_mixed_axis_label(t('Day (November 2009) and time (am/pm)'), 50);


//chart_2 = colour
//
// now make the chart
$chart_2 = array(
    '#chart_id' => 'colour_chart',
    '#title' => chart_title(t('Bananas colour'), 'cc0000', 15),
    '#type' => CHART_TYPE_BAR_V_GROUPED,
    '#size' => chart_size(940, 250),
    '#adjust_resolution' => TRUE,
    '#bar_size' => chart_bar_size(8, 2),
);

// use separate arrays for each data set (optional)
$op = array();
$ou = array();
$vp = array();
$vu = array();

$counter = 0;
foreach ($result[8] as $item) {

    if ($item == 'Yellow / green') {
        $val = '1';
    }
    elseif ($item == 'Yellow / Black patches') {
        $val = '2';
    }
    elseif (($item == 'Yellow / quite black') OR ($item == 'Yellow/ quite black')) {
        $val = '3';
    }
    elseif ($item == 'Yellow / Very black') {
        $val = '4';
    }
    elseif ($item == 'Mostly black now') {
        $val = '5';
    }
    elseif ($item == 'Really Black') {
        $val = '6';
    }
    else {
        $val = '0';
    };

    $remainder = $counter % 4;

    if ($remainder == 0) {
        $op[] = $val;
    }
    elseif ($remainder == 1) {
        $ou[] = $val;
    }
    elseif ($remainder == 2) {
        $vp[] = $val;
    }
    elseif ($remainder == 3) {
        $vu[] = $val;
    };
    $counter = $counter + 1;
};

// add data sets to chart
$chart_2['#data']['op'] = $op;
$chart_2['#data']['ou'] = $ou;
$chart_2['#data']['vp'] = $vp;
$chart_2['#data']['vu'] = $vu;

// add chart legend for each data set
$chart_2['#legends'][] = 'Organic packaged';
$chart_2['#legends'][] = 'Organic unpackaged';
$chart_2['#legends'][] = 'Value packaged';
$chart_2['#legends'][] = 'Value unpackaged';

// colour each data set differently
$chart_2['#data_colors'][] = 'ff0000';
$chart_2['#data_colors'][] = '00ff00';
$chart_2['#data_colors'][] = '0000ff';
$chart_2['#data_colors'][] = 'F4C400';

// add y axis labels
$chart_2['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][0][] = chart_mixed_axis_label(t(''));
$chart_2['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][0][] = chart_mixed_axis_label(t('YG'));
$chart_2['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][0][] = chart_mixed_axis_label(t('BP'));
$chart_2['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][0][] = chart_mixed_axis_label(t('QB'));
$chart_2['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][0][] = chart_mixed_axis_label(t('VB'));
$chart_2['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][0][] = chart_mixed_axis_label(t('MB'));
$chart_2['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][0][] = chart_mixed_axis_label(t('RB'));

$chart_2['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][1][] = chart_mixed_axis_label(t('Colour'), 90);

// add x axis labels
$chart_2['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('05 pm'));
$chart_2['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('06 am'));
$chart_2['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('06 pm'));
$chart_2['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('07 am'));
$chart_2['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('07 pm'));
$chart_2['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('08 am'));
$chart_2['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('08 pm'));
$chart_2['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('09 am'));
$chart_2['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('09 pm'));
$chart_2['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('10 am'));
$chart_2['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('10 pm'));
$chart_2['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('11 am'));
$chart_2['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('11 pm'));
$chart_2['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('12 am'));
$chart_2['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('12 pm'));
$chart_2['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('13 am'));
$chart_2['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('13 pm'));

$chart_2['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][3][] = chart_mixed_axis_label(t('Day (November 2009) and time (am/pm)'), 50);


// render chart
$output .= chart_render($chart_1);

// render chart
$output .= chart_render($chart_2);

// render (ordered) data table
$output .= theme('table', $headings, $flipped_results);


// display output = table and chart
echo $output;


?>