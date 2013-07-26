

nQuireJsSupport.register('NutritionalInformationMeasureData', {
  foods: {
    1: {title: "Potato", 'portion': "portion", 'value': [30, 3, 0, 2, 141, 12, 9, 1, 0, 1, 10, 0]},
    2: {title: "Pasta", 'portion': "portion", 'value': [45, 9, 2, 3, 86, 22, 52, 1, 0, 0, 0, 0]},
    3: {title: "Rice", 'portion': "portion", 'value': [56, 5, 2, 0, 122, 2, 32, 0, 0, 0, 0, 0]},
    4: {title: "Bread - White", 'portion': "slice", 'value': [13, 2, 1, 0, 10, 140, 30, 0, 0, 0, 0, 0]},
    5: {title: "Bread - Brown", 'portion': "slice", 'value': [13, 2, 1, 1, 12, 133, 56, 1, 0, 0, 0, 0]},
    6: {title: "Meat - Red", 'portion': "portion", 'value': [0, 12, 42, 0, 27, 28, 5, 1, 0, 2, 0, 0]},
    7: {title: "Meat - White", 'portion': "portion", 'value': [0, 22, 6, 0, 52, 64, 9, 1, 0, 0, 0, 0]},
    8: {title: "Fish", 'portion': "portion", 'value': [0, 26, 1, 0, 92, 408, 13, 0, 0, 3, 0, 0]},
    9: {title: "Pulses (peas, beans, lentils)", 'portion': "portion", 'value': [7, 3, 0, 1, 29, 5, 6, 1, 0, 0, 0, 0]},
    10: {title: "Egg", 'portion': "portion", 'value': [0, 7, 6, 0, 44, 83, 34, 1, 0, 1, 0, 1]},
    11: {title: "Cheese", 'portion': "portion", 'value': [15, 4, 10, 0, 20, 284, 60, 1, 0, 0, 0, 1]},
    12: {title: "Yogurt", 'portion': "portion", 'value': [17, 2, 1, 0, 0, 24, 49, 0, 0, 0, 44, 0]},
    13: {title: "Milk", 'portion': "glass", 'value': [14, 10, 5, 0, 269, 129, 360, 0, 0, 2, 3, 0]},
    14: {title: "Chocolate", 'portion': "bar", 'value': [34, 3, 15, 1, 0, 3, 18, 1, 0, 0, 0, 0]},
    15: {title: "Biscuits", 'portion': "portion", 'value': [42, 4, 15, 1, 2, 228, 54, 1, 0, 0, 0, 0]},
    16: {title: "Cake", 'portion': "slice", 'value': [33, 4, 16, 0, 10, 299, 64, 1, 0, 0, 0, 1]},
    17: {title: "Crisps", 'portion': "packet", 'value': [21, 2, 14, 2, 1, 320, 12, 1, 0, 0, 14, 0]},
    18: {title: "Vegetables", 'portion': "portion", 'value': [1, 1, 0, 1, 76, 5, 15, 0, 0, 0, 29, 0]},
    19: {title: "Fruit", 'portion': "portion", 'value': [12, 0, 0, 2, 85, 3, 4, 0, 0, 0, 6, 0]},
    20: {title: "Cereals", 'portion': "bowl", 'value': [45, 4, 1, 2, 1, 500, 2, 4, 0, 1, 0, 0]},
    21: {title: "Ice Cream", 'portion': "bowl", 'value': [24, 4, 12, 0, 75, 72, 120, 0, 109, 1, 1, 0]},
    22: {title: "General Pudding", 'portion': "bowl", 'value': [53, 3, 10, 2, 78, 99, 42, 1, 0, 0, 7, 0]}
  },
  components: ["carb", "protein", "fats", "fibre", "water", "sodium", "calcium", "iron", "vita", "vitb", "vitc", "vitd"],
  units: ["g", "g", "g", "g", "g", "mg", "mg", "mg", "mg", "mg", "mg", "mg"],
  init: function() {
  }
});