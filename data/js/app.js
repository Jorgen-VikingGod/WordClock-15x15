// used when hosting the site somewhere other than the ESP8266 (handy for
// testing without waiting forever to upload to SPIFFS)
var address = ip; //'192.168.188.65';
var urlBase = 'http://' + address + '/';

var postColorTimer = {};
var postValueTimer = {};

var ignoreColorChange = false;

var ws = new ReconnectingWebSocket('ws://' + address + ':81/', ['arduino']);
ws.debug = true;

var sections = [];

ws.onmessage = function (evt) {
  if (evt.data !== null) {
    var data = JSON.parse(evt.data);
    if (data === null) return;
    updateFieldValue(data.name, data.value);
  }
};

function inList(list, item) {
  $.each(list, function (index, field) {
    if (field.name == item.name) {
      return field.id;
    }
  });
}

function showElement(element) {
  element.addClass('d-flex').removeClass('d-none');
  // element.attr('style', element.attr('style') + 'display: block
  // !important;');
}

function hideElement(element) {
  element.removeClass('d-flex').addClass('d-none');
  // element.attr('style', element.attr('style') + 'display: none !important;');
}

$(document).ready(function () {
  $('#status').html('Connecting, please wait...');

  $.get(urlBase + 'all', function (data) {
    $('#status').html('Loading, please wait...');
    console.log(data);
    $.each(data, function (index, field) {
      var sectionId = '#container';
      if (field.section.name != '') {
        if (sections.length > 0) {
          $.each(sections, function (idx, item) {
            if (item.name == field.section.name) {
              sectionId = item.panel;
            }
          });
        }
        if (sectionId == '#container' || sectionId == '') {
          sectionId = addSectionField(field).panel;
        }
      }
      console.log(sectionId);
      if (field.type == 'Number') {
        addNumberField(sectionId, field);
      } else if (field.type == 'Boolean') {
        addBooleanField(sectionId, field);
      } else if (field.type == 'Select') {
        addSelectField(sectionId, field);
      } else if (field.type == 'Color') {
        addColorFieldPalette(sectionId, field);
        addColorFieldPicker(sectionId, field);
      } else if (field.type == 'Buttons') {
        addButtonsField(sectionId, field);
      }
    });

    $('.minicolors').minicolors({
      theme: 'bootstrap',
      changeDelay: 200,
      control: 'wheel',
      format: 'rgb',
      inline: true,
    });

    $('#status').html('Ready');
  }).fail(function (errorThrown) {
    console.log('error: ' + errorThrown);
  });
});

function addNumberField(sectionId, field) {
  var template = $('#numberTemplate').clone();

  template.attr('id', 'form-group-' + field.name);
  template.attr('data-field-type', field.type);

  var label = template.find('.control-label');
  label.attr('for', 'input-' + field.name);
  label.text(field.label);

  var input = template.find('.input');
  var slider = template.find('.custom-range');
  slider.attr('id', 'input-' + field.name);
  if (field.min) {
    input.attr('min', field.min);
    slider.attr('min', field.min);
  }
  if (field.max) {
    input.attr('max', field.max);
    slider.attr('max', field.max);
  }
  if (field.step) {
    input.attr('step', field.step);
    slider.attr('step', field.step);
  }
  input.val(field.value);
  slider.val(field.value);

  slider.on('change mousemove', function () {
    input.val($(this).val());
  });

  slider.on('change', function () {
    var value = $(this).val();
    input.val(value);
    field.value = value;
    delayPostValue(field.name, value);
  });

  input.on('change', function () {
    var value = $(this).val();
    slider.val(value);
    field.value = value;
    delayPostValue(field.name, value);
  });

  $(sectionId).append(template);
  console.debug('added number field');
}

function addBooleanField(sectionId, field) {
  var template = $('#booleanTemplate').clone();

  template.attr('id', 'form-group-' + field.name);
  template.attr('data-field-type', field.type);

  var label = template.find('.control-label');
  label.attr('for', 'btn-group-' + field.name);
  label.text(field.label);

  var btngroup = template.find('.btn-group');
  btngroup.attr('id', 'btn-group-' + field.name);

  var btnOn = template.find('#btnOn');
  var btnOff = template.find('#btnOff');

  btnOn.attr('id', 'btnOn' + field.name);
  btnOff.attr('id', 'btnOff' + field.name);

  btnOn.attr('class', field.value ? 'btn btn-primary' : 'btn');
  btnOff.attr('class', !field.value ? 'btn btn-primary' : 'btn');

  btnOn.click(function () {
    setBooleanFieldValue(field, btnOn, btnOff, 1);
  });
  btnOff.click(function () {
    setBooleanFieldValue(field, btnOn, btnOff, 0);
  });
  $(sectionId).append(template);
}

function toggleForegroundColorSelector(field) {
  var filterArray = ['solid'];
  if (filterArray.indexOf(field.toLowerCase()) > -1) {
    $('#form-group-foregroundColor').show();
    showElement($('#form-group-select-foregroundColor'));
  } else {
    $('#form-group-foregroundColor').hide();
    hideElement($('#form-group-select-foregroundColor'));
  }
}

function toggleBackgroundColorSelector(field) {
  var filterArray = ['solid color'];
  if (filterArray.indexOf(field.toLowerCase()) > -1) {
    $('#form-group-backgroundColor').show();
    showElement($('#form-group-select-backgroundColor'));
  } else {
    $('#form-group-backgroundColor').hide();
    hideElement($('#form-group-select-backgroundColor'));
  }
}

function toggleNoisePaletteSelector(field) {
  var filterArray = ['palette noise'];
  if (filterArray.indexOf(field.toLowerCase()) > -1) {
    showElement($('#form-group-noisePaletteSpeedX'));
    showElement($('#form-group-noisePaletteSpeedY'));
    showElement($('#form-group-noisePaletteSpeedZ'));
  } else {
    hideElement($('#form-group-noisePaletteSpeedX'));
    hideElement($('#form-group-noisePaletteSpeedY'));
    hideElement($('#form-group-noisePaletteSpeedZ'));
  }
}

function togglePalletteSelector(field) {
  var filterArray = ['palette noise'];
  if (filterArray.indexOf(field.toLowerCase()) > -1) {
    showElement($('#form-group-palette'));
    showElement($('#form-group-cyclePalettes'));
    showElement($('#form-group-paletteDuration'));
  } else {
    hideElement($('#form-group-palette'));
    hideElement($('#form-group-cyclePalettes'));
    hideElement($('#form-group-paletteDuration'));
  }
  if (field.toLowerCase() == 'black') {
    hideElement($('#form-group-backgroundBrightness'));
  } else {
    showElement($('#form-group-backgroundBrightness'));
  }
}

function toggleTourette(field, value) {
  if (field.toLowerCase() === 'tourette' && value === 1) {
    $('#form-group-section-tourette').show();
    showElement($('#form-group-touretteModes'));
    showElement($('#form-group-randomTouretteMode'));
    showElement($('#form-group-cycleTouretteMode'));
    showElement($('#form-group-touretteModeDuration'));
    toggleTouretteSelector($('#input-touretteModes option:selected').text());
  } else {
    $('#form-group-section-tourette').hide();
    hideElement($('#form-group-touretteModes'));
    hideElement($('#form-group-randomTouretteMode'));
    hideElement($('#form-group-cycleTouretteMode'));
    hideElement($('#form-group-touretteModeDuration'));
    toggleTouretteSelector('Off');
  }
}

function toggleTouretteSelector(field) {
  if (field.toLowerCase() === 'start') {
    showElement($('#form-group-touretteStartWords'));
    showElement($('#form-group-randomTouretteStart'));
    showElement($('#form-group-cycleTouretteStart'));
    showElement($('#form-group-touretteStartDuration'));

    showElement($('#form-group-touretteMiddleWords'));
    showElement($('#form-group-randomTouretteMiddle'));
    showElement($('#form-group-cycleTouretteMiddle'));
    showElement($('#form-group-touretteMiddleDuration'));

    hideElement($('#form-group-touretteEndWords'));
    hideElement($('#form-group-randomTouretteEnd'));
    hideElement($('#form-group-cycleTouretteEnd'));
    hideElement($('#form-group-touretteEndDuration'));
  } else if (field.toLowerCase() === 'end') {
    hideElement($('#form-group-touretteStartWords'));
    hideElement($('#form-group-randomTouretteStart'));
    hideElement($('#form-group-cycleTouretteStart'));
    hideElement($('#form-group-touretteStartDuration'));

    showElement($('#form-group-touretteMiddleWords'));
    showElement($('#form-group-randomTouretteMiddle'));
    showElement($('#form-group-cycleTouretteMiddle'));
    showElement($('#form-group-touretteMiddleDuration'));

    showElement($('#form-group-touretteEndWords'));
    showElement($('#form-group-randomTouretteEnd'));
    showElement($('#form-group-cycleTouretteEnd'));
    showElement($('#form-group-touretteEndDuration'));
  } else {
    hideElement($('#form-group-touretteStartWords'));
    hideElement($('#form-group-randomTouretteStart'));
    hideElement($('#form-group-cycleTouretteStart'));
    hideElement($('#form-group-touretteStartDuration'));

    hideElement($('#form-group-touretteMiddleWords'));
    hideElement($('#form-group-randomTouretteMiddle'));
    hideElement($('#form-group-cycleTouretteMiddle'));
    hideElement($('#form-group-touretteMiddleDuration'));

    hideElement($('#form-group-touretteEndWords'));
    hideElement($('#form-group-randomTouretteEnd'));
    hideElement($('#form-group-cycleTouretteEnd'));
    hideElement($('#form-group-touretteEndDuration'));
  }
}

function toggleSections(field) {
  toggleBackgroundColorSelector(field);
  toggleNoisePaletteSelector(field);
  togglePalletteSelector(field);
}

$.fn.toggleSections = function (field) {
  toggleSections(field);
};

function addSelectField(sectionId, field) {
  var template = $('#selectTemplate').clone();

  template.attr('id', 'form-group-' + field.name);
  template.attr('data-field-type', field.type);

  var id = 'input-' + field.name;

  var label = template.find('.control-label');
  label.attr('for', id);
  label.text(field.label);

  var select = template.find('.form-control');
  select.attr('id', id);

  for (var i = 0; i < field.options.length; i++) {
    var optionText = field.options[i];
    var option = $('<option></option>');
    option.text(optionText);
    option.attr('value', i);
    select.append(option);
  }

  select.val(field.value);

  select.change(function () {
    var value = template.find('#' + id + ' option:selected').index();
    postValue(field.name, value);
    if (id == 'input-background') {
      toggleSections(field.options[select.val()]);
    } else if (id == 'input-touretteModes') {
      toggleTouretteSelector(field.options[select.val()]);
    } else if (id == 'input-colorEffect') {
      toggleForegroundColorSelector(field.options[select.val()]);
    }
  });

  var previousButton = template.find('.btn-previous');
  var nextButton = template.find('.btn-next');

  previousButton.click(function () {
    var value = template.find('#' + id + ' option:selected').index();
    var count = select.find('option').length;
    value--;
    if (value < 0) value = count - 1;
    select.val(value);
    postValue(field.name, value);
    if (id == 'input-background') {
      toggleSections(field.options[select.val()]);
    } else if (id == 'input-touretteModes') {
      toggleTouretteSelector(field.options[select.val()]);
    } else if (id == 'input-colorEffect') {
      toggleForegroundColorSelector(field.options[select.val()]);
    }
  });

  nextButton.click(function () {
    var value = template.find('#' + id + ' option:selected').index();
    var count = select.find('option').length;
    value++;
    if (value >= count) value = 0;
    select.val(value);
    postValue(field.name, value);
    if (id == 'input-background') {
      toggleSections(field.options[select.val()]);
    } else if (id == 'input-touretteModes') {
      toggleTouretteSelector(field.options[select.val()]);
    } else if (id == 'input-colorEffect') {
      toggleForegroundColorSelector(field.options[select.val()]);
    }
  });
  $(sectionId).append(template).toggleSections(field.options[select.val()]);
}

function addColorFieldPicker(sectionId, field) {
  var template = $('#colorTemplate').clone();

  template.attr('id', 'form-group-' + field.name);
  template.attr('data-field-type', field.type);

  var id = 'input-' + field.name;

  var input = template.find('.minicolors');
  input.attr('id', id);

  if (!field.value.startsWith('rgb(')) field.value = 'rgb(' + field.value;

  if (!field.value.endsWith(')')) field.value += ')';

  input.val(field.value);

  var components = rgbToComponents(field.value);

  var redInput = template.find('.color-red-input');
  var greenInput = template.find('.color-green-input');
  var blueInput = template.find('.color-blue-input');

  var redSlider = template.find('.color-red-slider');
  var greenSlider = template.find('.color-green-slider');
  var blueSlider = template.find('.color-blue-slider');

  redInput.attr('id', id + '-red');
  greenInput.attr('id', id + '-green');
  blueInput.attr('id', id + '-blue');

  redSlider.attr('id', id + '-red-slider');
  greenSlider.attr('id', id + '-green-slider');
  blueSlider.attr('id', id + '-blue-slider');

  redInput.val(components.r);
  greenInput.val(components.g);
  blueInput.val(components.b);

  redSlider.val(components.r);
  greenSlider.val(components.g);
  blueSlider.val(components.b);

  redInput.on('change', function () {
    var value = $('#' + id).val();
    var r = $(this).val();
    var components = rgbToComponents(value);
    field.value = r + ',' + components.g + ',' + components.b;
    $('#' + id).minicolors('value', 'rgb(' + field.value + ')');
    redSlider.val(r);
  });

  greenInput.on('change', function () {
    var value = $('#' + id).val();
    var g = $(this).val();
    var components = rgbToComponents(value);
    field.value = components.r + ',' + g + ',' + components.b;
    $('#' + id).minicolors('value', 'rgb(' + field.value + ')');
    greenSlider.val(g);
  });

  blueInput.on('change', function () {
    var value = $('#' + id).val();
    var b = $(this).val();
    var components = rgbToComponents(value);
    field.value = components.r + ',' + components.g + ',' + b;
    $('#' + id).minicolors('value', 'rgb(' + field.value + ')');
    blueSlider.val(b);
  });

  redSlider.on('change', function () {
    var value = $('#' + id).val();
    var r = $(this).val();
    var components = rgbToComponents(value);
    field.value = r + ',' + components.g + ',' + components.b;
    $('#' + id).minicolors('value', 'rgb(' + field.value + ')');
    redInput.val(r);
  });

  greenSlider.on('change', function () {
    var value = $('#' + id).val();
    var g = $(this).val();
    var components = rgbToComponents(value);
    field.value = components.r + ',' + g + ',' + components.b;
    $('#' + id).minicolors('value', 'rgb(' + field.value + ')');
    greenInput.val(g);
  });

  blueSlider.on('change', function () {
    var value = $('#' + id).val();
    var b = $(this).val();
    var components = rgbToComponents(value);
    field.value = components.r + ',' + components.g + ',' + b;
    $('#' + id).minicolors('value', 'rgb(' + field.value + ')');
    blueInput.val(b);
  });

  redSlider.on('change mousemove', function () {
    redInput.val($(this).val());
  });

  greenSlider.on('change mousemove', function () {
    greenInput.val($(this).val());
  });

  blueSlider.on('change mousemove', function () {
    blueInput.val($(this).val());
  });

  input.on('change', function () {
    if (ignoreColorChange) return;

    var value = $(this).val();
    var components = rgbToComponents(value);

    redInput.val(components.r);
    greenInput.val(components.g);
    blueInput.val(components.b);

    redSlider.val(components.r);
    greenSlider.val(components.g);
    blueSlider.val(components.b);

    field.value = components.r + ',' + components.g + ',' + components.b;
    delayPostColor(field.name, components);
  });

  $(sectionId).append(template);
}

function addColorFieldPalette(sectionId, field) {
  var template = $('#colorPaletteTemplate').clone();

  template.attr('id', 'form-group-select-' + field.name);

  var buttons = template.find('.btn-color');

  var label = template.find('.control-label');
  label.text(field.label);

  buttons.each(function (index, button) {
    $(button).click(function () {
      var rgb = $(this).css('backgroundColor');
      var components = rgbToComponents(rgb);

      field.value = components.r + ',' + components.g + ',' + components.b;
      postColor(field.name, components);

      ignoreColorChange = true;
      var id = '#input-' + field.name;
      $(id).minicolors('value', 'rgb(' + field.value + ')');
      $(id + '-red').val(components.r);
      $(id + '-green').val(components.g);
      $(id + '-blue').val(components.b);
      $(id + '-red-slider').val(components.r);
      $(id + '-green-slider').val(components.g);
      $(id + '-blue-slider').val(components.b);
      ignoreColorChange = false;
    });
  });

  $(sectionId).append(template);

  setTimeout(function () {
    // jquery is weird, let it init then hide it.
    toggleForegroundColorSelector(
      $('#input-colorEffect option:selected').text()
    );
    toggleSections($('#input-background option:selected').text());
    toggleTouretteSelector($('#input-touretteModes option:selected').text());
  }, 0);
}

function addButtonsField(sectionId, field) {
  var template = $('#buttonsTemplate').clone();

  template.attr('id', 'form-group-' + field.name);
  template.attr('data-field-type', field.type);

  var label = template.find('p');
  label.text(field.label);
  var buttonGroup = template.find('div');

  for (var i = 0; i < field.options.length; i++) {
    var buttonTemplate = $('#buttonsTemplateButton').clone();
    var optionText = field.options[i];
    var button = buttonTemplate.find('button');
    button.attr('id', 'form-group-' + field.name + 'button' + i);
    button.attr('value', i);
    button.text(optionText);
    if (i == field.value) {
      button.addClass('active');
    }
    buttonGroup.append(buttonTemplate);
    button.click(function () {
      postValue(field.name, i);
    });
  }

  $(sectionId).append(template);
}

function addSectionField(field) {
  var sectionName = field.section.name;
  var section = sectionName.toLowerCase().replace(' ', '-');
  var sectionID = '#form-group-section-' + section;
  if ($(sectionID).length > 0) {
    console.log('already exists: ', 'form-group-section-' + section);
    return '#container';
  } else {
    var template = $('#sectionTemplate').clone();
    template.attr('id', 'form-group-section-' + section);
    template.addClass(field.section.layout);
    template.attr('data-field-type', field.type);
    var panel = template.find('.card');
    panel.addClass(field.section.color);
    var panelTitle = template.find('.card-header');
    var panelBody = template.find('.card-body');
    panelTitle.text(sectionName);
    panelBody.attr('id', 'form-group-section-panel-' + section);
    console.log('create: ', 'form-group-section-' + section);
    var sectionObj = {
      name: sectionName,
      id: sectionID,
      panel: '#form-group-section-panel-' + section,
    };
    sections.push(sectionObj);
    $('#container').append(template);
    return sectionObj;
  }
}

function updateFieldValue(name, value) {
  if (name == 'time') {
    $('#time').html(value);
  } else {
    var group = $('#form-group-' + name);

    var type = group.attr('data-field-type');

    if (type == 'Number') {
      var input = group.find('.form-control');
      input.val(value);
    } else if (type == 'Boolean') {
      var btnOn = group.find('#btnOn' + name);
      var btnOff = group.find('#btnOff' + name);

      btnOn.attr('class', value ? 'btn btn-primary' : 'btn');
      btnOff.attr('class', !value ? 'btn btn-primary' : 'btn');
      if (name.toLowerCase() === 'tourette') {
        toggleTourette(name, value);
      }
    } else if (type == 'Select') {
      var select = group.find('.form-control');
      select.val(value);
      if (name == 'background') {
        toggleSections($('#input-background option:selected').text());
      } else if (name == 'touretteModes') {
        toggleTouretteSelector(
          $('#input-touretteModes option:selected').text()
        );
      } else if (name == 'colorEffect') {
        toggleForegroundColorSelector(
          $('#input-colorEffect option:selected').text()
        );
      }
    } else if (type == 'Color') {
      var input = group.find('.form-control');
      input.val('rgb(' + value + ')');
    }
  }
}

function setBooleanFieldValue(field, btnOn, btnOff, value) {
  field.value = value;

  btnOn.attr('class', field.value ? 'btn btn-primary' : 'btn');
  btnOff.attr('class', !field.value ? 'btn btn-primary' : 'btn');
  postValue(field.name, field.value);
}

function postValue(name, value) {
  $('#status').html('Setting ' + name + ': ' + value + ', please wait...');

  var body = {
    name: name,
    value: value,
  };

  $.post(
    urlBase + 'fieldValue' + '?value=' + value + '&name=' + name,
    body,
    function (data) {
      if (data.name != null) {
        $('#status').html('Set ' + name + ': ' + data.name);
      } else {
        $('#status').html('Set ' + name + ': ' + data);
      }
    }
  );
}

function delayPostValue(name, value) {
  clearTimeout(postValueTimer);
  postValueTimer = setTimeout(function () {
    postValue(name, value);
  }, 300);
}

function postColor(name, value) {
  $('#status').html(
    'Setting ' +
      name +
      ': ' +
      value.r +
      ',' +
      value.g +
      ',' +
      value.b +
      ', please wait...'
  );

  var body = {
    name: name,
    r: value.r,
    g: value.g,
    b: value.b,
  };

  $.post(
    urlBase +
      'fieldValue?name=' +
      name +
      '&r=' +
      value.r +
      '&g=' +
      value.g +
      '&b=' +
      value.b,
    body,
    function (data) {
      $('#status').html('Set ' + name + ': ' + data);
    }
  ).fail(function (textStatus, errorThrown) {
    $('#status').html('Fail: ' + textStatus + ' ' + errorThrown);
  });
}

function delayPostColor(name, value) {
  clearTimeout(postColorTimer);
  postColorTimer = setTimeout(function () {
    postColor(name, value);
  }, 300);
}

function componentToHex(c) {
  var hex = c.toString(16);
  return hex.length == 1 ? '0' + hex : hex;
}

function rgbToHex(r, g, b) {
  return '#' + componentToHex(r) + componentToHex(g) + componentToHex(b);
}

function rgbToComponents(rgb) {
  var components = {};

  rgb = rgb.match(/^rgb\((\d+),\s*(\d+),\s*(\d+)\)$/);
  components.r = parseInt(rgb[1]);
  components.g = parseInt(rgb[2]);
  components.b = parseInt(rgb[3]);

  return components;
}
