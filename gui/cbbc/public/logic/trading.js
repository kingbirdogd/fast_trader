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

const formatLong = (val) => {
  var unit = 100000000
  if (typeof val !='undefined' && val>0)
    return parseFloat(val)/unit
  else
    return 0.00
}

const formatLongV2 = (val) => {
  var unit = 100000000
  return parseFloat(val)*unit
}

const formatPrice = (val) => {
  var unit = 100000
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

const formatInputUnit = (value, is_change_digital) => {
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