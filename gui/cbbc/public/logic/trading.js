const validate = (lang, valid, val, isCheckNull) => {
  var text = {
    en: {empty: 'Empty', zero: 'Zero', number: 'Number', code: 'Cbbc Code'},
    sc: {empty: '不能空', zero: '不能是0', number: '必须是数字', code: '必须是牛熊证'},
    tc: {empty: '不能空', zero: '不能是0', number: '必須是數字', code: '必須是牛熊證'}
  }
  
  if(isCheckNull && (typeof val == "undefined" || val.length==0))
    return text[lang].empty
  if(val.length>0 && typeof val !== "undefined") {
    if(valid=='code' && ((val*1 < 10000 || val*1 > 100000) || isNaN(val)))
      return text[lang].code
    else if ((valid=='number' || valid=='code' || valid=='ucode') && val*1==0)
      return text[lang].zero
    else if((valid=='number' || valid=='number_except_zero' || valid=='code' || valid=='ucode') && isNaN(val))
      return text[lang].number
  }
  return ""
}

const validate2 = (lang, state, fields) => {
  var text = {
    en: {empty: 'Empty', zero: 'Zero', number: 'Number', code: 'Warrant/Cbbc', ucode: 'Underlying'},
    sc: {empty: '不能空', zero: '不能是0', number: '必须是数字', code: '必须是权证', ucode: '必须是正股'},
    tc: {empty: '不能空', zero: '不能是0', number: '必須是數字', code: '必須是權證', ucode: '必須是正股'}
  }
  
  var error = {}
  for(var i in fields) {
    var {name, valid} = fields[i]
    var val = state.data[name]
    if((valid=='string' || valid=='number' || valid=='code' || valid=='ucode') 
        && (!state.data.hasOwnProperty(name) || val.length==0) )
      error[name] = text[lang].empty
    else if ((valid=='number' || valid=='code' || valid=='ucode') && val*1==0)
      error[name] = text[lang].zero
    else if((valid=='number' || valid=='number_except_zero' || valid=='code' || valid=='ucode') && isNaN(val))
      error[name] = text[lang].number
    else if(valid=='code' && (val*1 < 10000 || val*1 > 100000))
      error[name] = text[lang].code
    else if(valid=='ucode' && val*1 > 9999)
      error[name] = text[lang].ucode
  }
  return error
}

const getUnderlyingName = (code) => {
  var ucodesName2 = {}, name = '', code = formatCode(code, 5)
  for (const [k, v] of Object.entries(ucodesName))
    for (const [k1, v1] of Object.entries(v))
      ucodesName2[k1] = v1
  if (code in ucodesName2)
    name = ucodesName2[code]
  return name
}

const getUnderlyingName2 = (code) => {
  if (!('underlyingName' in global)) global.underlyingName = {}
  if (parseInt(code) in global.underlyingName)
    return global.underlyingName[parseInt(code)]
  for(var v of ucodesName2)
    if (parseInt(v.code) == parseInt(code)) {
      global.underlyingName[parseInt(v.code)] = v.nmll
      return v.nmll
    }
  return ''
}

const formatCommand = (key, state, fields, original) => {
  var command = new Array()
  var unit = 100000000
  for (var i in original) {
    var k = Object.keys(fields).find(key => fields[key].name === original[i])
    var {name, valid} = fields[k]
    if(valid=='string' || valid=='code' || valid=='ucode')
      command.push(state[name])
    else if(valid=='number')
      command.push(state[name]*unit)
  }
  command.unshift('set')
  command.push(key)
  return command
}

const formatLong = (val, digit=3) => {
  var val = parseFloat(val);
  var unit = parseInt(100000000);
  if (typeof val !='undefined' && val != null && val != '' && val>0)
    return parseFloat(parseFloat(val/unit).toFixed(digit))
  else
    return 0.00
}

const formatLongV2 = (val, digit=0) => {
  var val = parseFloat(val);
  var unit = parseInt(100000000);
  if (typeof val !='undefined' && val != null && val != '' && val>0)
    return parseFloat(parseFloat(val*unit).toFixed(digit))
  else
    return 0.00
}

const formatPrice = (val) => {
  var unit = 100000
  return parseFloat(val)/unit
}

const formatPrice2 = (val) => {
  var unit = 1000
  return parseFloat(val)/unit
}

const numberWithCommas = (x) => {
  if (x.length==0)
    return ''
  
  var arr = parseFloat(x).toString().split('.')
  var temp1 = arr[0].toString().replace(/\B(?=(\d{3})+(?!\d))/g, ",")
  if(arr.length==1)
    return temp1
  else if (arr.length==2)
    return temp1+'.'+arr[1]
  else
    return x
}

const formatDate = (str) => {
  return str.substring(0, 4)+'-'+str.substring(4, 6)+'-'+str.substring(6, 8)+' '+str.substring(8, 10)+':'+str.substring(10, 12)+':'+str.substring(12, 14)
}

const getCurDateTime = () => {
  var today = new Date()
  var dd = String(today.getDate()).padStart(2, '0')
  var mm = String(today.getMonth() + 1).padStart(2, '0')
  var yyyy = today.getFullYear()
  
  var hh = String(today.getHours()).padStart(2, '0')
  var mm = String(today.getMinutes()).padStart(2, '0')
  var ss = String(today.getSeconds()).padStart(2, '0')
  
  return yyyy+"-"+mm+"-"+dd+" "+hh+":"+mm+ ":"+ss
}

const formatInput = (value) => {
  var temp = value.replace(/\s/g, '')
  var removes = ['~','!','@','#','%','^','&','*','-','+','_','[',']','"','\'','<','>','/',',','{','}',';','`']
  removes.forEach(function(ele) {
    temp = temp.replace(ele, '')
  })
  return temp
}

const formatInput2 = (value) => {
  if (!value)
    return value
  var temp = value.replace(/\s/g, '').split('')
  var temp2 = ''
  var allows = ['1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '.', 'k', 'm', 'b']
  for (var i in temp)
    if (allows.includes(temp[i])) temp2 += temp[i]
  return temp2
}

const formatInputUnit = (value, is_change_digital) => {
  if (typeof value !== 'undefined') {
    var val = value.toString().toLowerCase(), temp = parseFloat(val)

    if (is_change_digital)
      if (val.includes('k'))
        return temp * 1000;
      else if (val.includes('m'))
        return temp * 1000000;
      else if (val.includes('b'))
        return temp * 1000000000
      else
        return temp
    
    else if (!is_change_digital)
      if (val.includes('k') || val.includes('m') || val.includes('b'))
        return value;
      else if (temp >= 1000000000)
        return (temp/1000000000)+'b'
      else if (temp >= 1000000)
        return (temp/1000000)+'m'
      else if (temp >= 1000)
        return (temp/1000)+'k'
      else
        return temp
  }
  return value
}

function getSpread(val){
  var spread = 0;
  if(val*1>=0.01 && val*1<0.25){
    spread = 0.001;
  }else if(val*1>=0.25 && val*1<0.5){
    spread = 0.005;
  }else if(val*1>=0.5 && val*1<10){
    spread = 0.01;
  }else if(val*1>=10 && val*1<20){
    spread = 0.02;
  }else if(val*1>=20 && val*1<100){
    spread = 0.05;
  }else if(val*1>=100 && val*1<200){
    spread = 0.1;
  }else if(val*1>=200 && val*1<500){
    spread = 0.2;
  }else if(val*1>=500 && val*1<1000){
    spread = 0.5;
  }else if(val*1>=1000 && val*1<2000){
    spread = 1;
  }else if(val*1>=2000 && val*1<5000){
    spread = 2;
  }else if(val*1>=5000){
    spread = 5;
  }else{
    spread = 0;
  }
  return spread;
}

function getSpreadStock(val){
  val = parseFloat(val);
  if (val>=0.01 && val<0.25) {return 0.001;}
  else if (val>=0.25 && val<0.5) {return 0.005;}
  else if (val>=0.5 && val<10) {return 0.01;}
  else if (val>=10 && val<20) {return 0.02;}
  else if (val>=20 && val<100) {return 0.05;}
  else if (val>=100 && val<200) {return 0.1;}
  else if (val>=200 && val<500) {return 0.2;}
  else if (val>=500 && val<1000) {return 0.5;}
  else if (val>=100 && val<2000) {return 1;}
  else if (val>=2000 && val<5000) {return 2;}
  else if (val>=5000 && val<9955) {return 5;}
  else {return 0;}
}

function getSpreadETF(val){
  val = parseFloat(val);
  if (val>=0.01 && val<0.25) {return 0.001;}
  else if (val>=0.25 && val<0.5) {return 0.002;}
  else if (val>=0.5 && val<10) {return 0.005;}
  else if (val>=10 && val<20) {return 0.01;}
  else if (val>=20 && val<100) {return 0.02;}
  else if (val>=100 && val<200) {return 0.05;}
  else if (val>=200 && val<500) {return 0.1;}
  else if (val>=500 && val<1000) {return 0.2;}
  else if (val>=100 && val<2000) {return 0.5;}
  else if (val>=2000 && val<5000) {return 1;}
  else if (val>=5000 && val<9955) {return 1;}
  else {return 0;}
}

function isETF(ucode) {
  ucode = parseInt(ucode);
  if ([2800, 3033, 2828, 2822, 3188].includes(ucode)) {return true}
  return false
}

function isIndex(ucode) {
  if (!ucode) {return false}
  for (var index of ['hsi', 'hsce', 'hti']) {
    if (ucode.toString().toLowerCase().includes(index))
      return true
  }
  return false
}

function capitalize(val) {
  if (!val) return val;
  return val.charAt(0).toUpperCase() + val.slice(1);
}

function getNo(val) {
  return parseInt(val.replace('u', '').replace('_1', ''));
}

function setNo(val) {
  return 'u'+val.toString().padStart(3, '0')+'_1'
}

function getTime() {
  return moment().format("HH:mm:ss")
}

function getIssuer() {
  return {
      BI: {sc: '中銀', tc: '中银', en: '中银'},
      BP: {sc: '法巴', tc: '法巴', en: '法巴'},
      CS: {sc: '瑞信', tc: '瑞信', en: '瑞信'},
      CT: {sc: '花旗', tc: '花旗', en: '花旗'},
      DS: {sc: '星展', tc: '星展', en: '星展'},
      EA: {sc: '東亞', tc: '東亞', en: '東亞'},
      GJ: {sc: '国君', tc: '國君', en: '国君'},
      GS: {sc: '高盛', tc: '高盛', en: '高盛'},
      HS: {sc: '汇丰', tc: '匯豐', en: '汇丰'},
      HT: {sc: '海通', tc: '海通', en: '海通'},
      JP: {sc: '摩通', tc: '摩通', en: '摩通'},
      MB: {sc: '麦银', tc: '麥銀', en: '麦银'},
      MS: {sc: '摩利', tc: '摩利', en: '摩利'},
      SG: {sc: '法兴', tc: '法興', en: '法兴'},
      UB: {sc: '瑞银', tc: '瑞銀', en: '瑞银'},
      VT: {sc: '瑞通', tc: '瑞通', en: '瑞通'}
    }
}

function playClickSound() {
  var audioElement = document.createElement('audio');
  audioElement.setAttribute('src', '/sound/Mouse-Click-00-m-FesliyanStudios.com.mp3');
  audioElement.setAttribute('autoplay', 'autoplay');
  audioElement.play();
}

const fakeFormSubmit = (fields) => {
  var $form = $('<form>', {
    action: 'https://chart.dbpower.com.hk/PHPExcel/download.php',
    method: 'post'
  });
  $.each(fields, function(key, val) {
    $('<input>').attr({
      type: "hidden",
      name: key,
      value: val
    }).appendTo($form);
  });
  $form.appendTo('body').submit();
}

function formatTableData(key) {
  var tb = $(key),
      data = []
      
  tb.find('tr').each(function() {
    var row = []
    // header
    $(this).find('th').each(function() {
      if ($(this).html().includes('span'))
        row.push($(this).children().html().replaceAll(',', ''))
      else
        row.push($(this).html().replaceAll(',', ''))
    })
    // content
    $(this).find('td').each(function() {
      row.push($(this).html().replaceAll(',', ''))
    })
    data.push(row)
  })
  return JSON.stringify(data).replaceAll('"', '').replaceAll('[[', '').replaceAll(']]', '').replaceAll('],[', '|')
}
