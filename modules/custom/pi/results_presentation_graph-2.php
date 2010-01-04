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


// now make the chart
$chart = array(
    '#chart_id' => 'test_chart',
    '#title' => chart_title(t('Bananas'), 'cc0000', 15),
    '#type' => CHART_TYPE_BAR_V_GROUPED,
    '#size' => chart_size(920, 300),
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
$chart['#data']['op'] = $op;
$chart['#data']['ou'] = $ou;
$chart['#data']['vp'] = $vp;
$chart['#data']['vu'] = $vu;

// add chart legend for each data set
$chart['#legends'][] = 'Organic packaged';
$chart['#legends'][] = 'Organic unpackaged';
$chart['#legends'][] = 'Value packaged';
$chart['#legends'][] = 'Value unpackaged';

// colour each data set differently
$chart['#data_colors'][] = '00ff00';
$chart['#data_colors'][] = 'ff0000';
$chart['#data_colors'][] = '0000ff';
$chart['#data_colors'][] = 'ffff00';

// add y axis labels
$chart['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][0][] = chart_mixed_axis_label(t('00'));
$chart['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][0][] = chart_mixed_axis_label(t('01'));
$chart['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][0][] = chart_mixed_axis_label(t('02'));
$chart['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][0][] = chart_mixed_axis_label(t('03'));
$chart['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][0][] = chart_mixed_axis_label(t('04'));
$chart['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][0][] = chart_mixed_axis_label(t('05'));
$chart['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][0][] = chart_mixed_axis_label(t('06'));
$chart['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][0][] = chart_mixed_axis_label(t('07'));

$chart['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][1][] = chart_mixed_axis_label(t('Smell'), 90);

// add x axis labels
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('05 pm'));
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('06 am'));
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('06 pm'));
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('07 am'));
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('07 pm'));
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('08 am'));
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('08 pm'));
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('09 am'));
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('09 pm'));
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('10 am'));
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('10 pm'));
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('11 am'));
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('11 pm'));
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('12 am'));
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('12 pm'));
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('13 am'));
$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('13 pm'));

$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][3][] = chart_mixed_axis_label(t('Day (November 2009) and time (am/pm)'), 50);


// render chart
$output .= chart_render($chart);

// render (ordered) data table
$output .= theme('table', $headings, $flipped_results);


// display output = table and chart
echo $output;


?>