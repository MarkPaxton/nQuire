<?php

global $user;
$node_details = get_inquiry_details_from_current_path();

$output = '';

// group's selected measures
$selected_measures = "490,491,492,494,495,493,541,549";

// get group data
$result = array();

foreach (explode(',', $selected_measures) as $measure_id) {

    if ($measure_id != 0) {
        $result[] = get_data_for_inquiry_measure_and_user ($node_details->inquiry_id, $measure_id, $user->uid);
    };
};

// sort group data
array_multisort(
    $result[2], SORT_ASC, SORT_STRING,
    $result[1], SORT_ASC, SORT_STRING,
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

// use separate arrays for each data set (optional)
$data = array();

$counter = 0;
foreach ($result[8] as $item) {

    if ($item == 'Little smell') {
        $val = '1';
    }
    elseif ($item == 'Some smell') {
        $val = '2';
    }
    elseif ($item == 'Quite strong smell') {
        $val = '3';
    }
    elseif ($item == 'Strong smell') {
        $val = '4';
    }
    elseif ($item == 'Extreme now') {
        $val = '5';
    }
    else {
        $val = '0';
    };

    $data[] = $val;

};

//chart = smell
//
// now make the chart
$chart = array(
    '#chart_id' => 'cheese_smell_chart',
    '#title' => chart_title(t('Cheese smell'), '00ee00', 15),
    '#type' => CHART_TYPE_BAR_V_GROUPED,
    '#size' => chart_size(940, 250),
    '#adjust_resolution' => TRUE,
    '#bar_size' => chart_bar_size(16, 2),
);

// add data sets to chart
$chart['#data']['data'] = $data;

// colour each data set differently
$chart['#data_colors'][] = 'ff0000';

// add y axis labels
$chart['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][0][] = chart_mixed_axis_label(t(''));
$chart['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][0][] = chart_mixed_axis_label(t('Little smell'));
$chart['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][0][] = chart_mixed_axis_label(t('Some smell'));
$chart['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][0][] = chart_mixed_axis_label(t('Quite string smell'));
$chart['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][0][] = chart_mixed_axis_label(t('Strong smell'));
$chart['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][0][] = chart_mixed_axis_label(t('Extreme smell'));

$chart['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][1][] = chart_mixed_axis_label(t('Smell'), 90);

// add x axis labels
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('14'));
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('15'));
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('21'));
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('09'));
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('11'));
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('13'));
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('19'));
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('19'));
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('21'));
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('23'));
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('19'));
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('28'));
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('29'));

$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][2][] = chart_mixed_axis_label(t('Boxed - Blue cheese'));
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][2][] = chart_mixed_axis_label(t(''));
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][2][] = chart_mixed_axis_label(t(''));
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][2][] = chart_mixed_axis_label(t('Open air - Blue cheese'));
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][2][] = chart_mixed_axis_label(t(''));
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][2][] = chart_mixed_axis_label(t(''));
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][2][] = chart_mixed_axis_label(t(''));
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][2][] = chart_mixed_axis_label(t(''));
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][2][] = chart_mixed_axis_label(t(''));
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][2][] = chart_mixed_axis_label(t('Open air - Cheddar cheese'));
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][2][] = chart_mixed_axis_label(t(''));
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][2][] = chart_mixed_axis_label(t(''));

$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][3][] = chart_mixed_axis_label(t('Day (November 2009)'), 50);


// render chart
$output .= chart_render($chart);

// render (ordered) data table
$output .= theme('table', $headings, $flipped_results);


// display output = table and chart
echo $output;


?>