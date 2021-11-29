class Strategy1 extends React.Component {
  static propTypes = {
    data: PropTypes.object,
    lang: PropTypes.string,
    setStates: PropTypes.func,
    getStates: PropTypes.func
  }
  
  constructor(props) {
    super(props)
    this.state = {}
    this.handleChange = this.handleChange.bind(this)
    this.handleBlur = this.handleBlur.bind(this)
    this.handleClick = this.handleClick.bind(this)
    this.handleClick2 = this.handleClick2.bind(this)
    this.sendMsg = this.sendMsg.bind(this)
  }
  
  static getDerivedStateFromProps(props, state) {
    return state
  }
  
  componentDidMount() {

  }
  
  handleChange() {
    var {name, value} = event.target,
        no = event.target.attributes.getNamedItem('data-no').value
    var states = this.props.getStates(),
        obj = $.extend(true, {}, this.props.data),
        text = this.getText(this.props.lang)
    
    // avoid to focus on trade table field
    global.dtTradeTableForces = null
        
    obj[name].value = formatInput2(value)
    obj[name].feedback = validate(this.props.lang, obj[name].valid, obj[name].value, false)
    obj[name].responseResult = ''
    
    if (obj[name].value.toString().length > 0)
      obj[name].value = parseInt(obj[name].value)
    else
      obj[name].value = ''
    
    // valid
    if (!obj[name].feedback) {
      var n = name.toLowerCase().replace(/[0-9]/g, '')
      
      // 大小位置
      if (n == 'bullrs' && parseInt(obj['bullr'+no+'s'].value) >= parseInt(obj['bullr'+no+'e'].value) && parseInt(obj['bullr'+no+'e'].value) > 0) 
        obj['bullr'+no+'s'].feedback = '<'+obj['bullr'+no+'e'].value
      
      else if (n == 'bullre' && parseInt(obj['bullr'+no+'e'].value) <= parseInt(obj['bullr'+no+'s'].value) && parseInt(obj['bullr'+no+'s'].value) > 0)
        obj['bullr'+no+'e'].feedback = '>'+obj['bullr'+no+'s'].value
      
      else if (n == 'bearrs' && parseInt(obj['bearr'+no+'s'].value) <= parseInt(obj['bearr'+no+'e'].value) && parseInt(obj['bearr'+no+'e'].value) > 0)
        obj['bearr'+no+'s'].feedback = '>'+obj['bearr'+no+'e'].value
      
      else if (n == 'bearre' && parseInt(obj['bearr'+no+'e'].value) >= parseInt(obj['bearr'+no+'s'].value) && parseInt(obj['bearr'+no+'s'].value) > 0)
        obj['bearr'+no+'e'].feedback = '<'+obj['bearr'+no+'s'].value
      
      // 必需一對
      if (parseInt(obj['bullr'+no+'s'].value) >= 0 && obj['bullr'+no+'e'].value.toString().length == 0)
        obj['bullr'+no+'e'].feedback = text.empty
      
      if (parseInt(obj['bullr'+no+'e'].value) >= 0 && obj['bullr'+no+'s'].value.toString().length == 0)
        obj['bullr'+no+'s'].feedback = text.empty
      
      if (parseInt(obj['bearr'+no+'s'].value) >= 0 && obj['bearr'+no+'e'].value.toString().length == 0)
        obj['bearr'+no+'e'].feedback = text.empty
      
      if (parseInt(obj['bearr'+no+'e'].value) >= 0 && obj['bearr'+no+'s'].value.toString().length == 0)
        obj['bearr'+no+'s'].feedback = text.empty
      
      // 需一對0
      if (parseInt(obj['bullr'+no+'s'].value) == 0 && parseInt(obj['bullr'+no+'e'].value) != 0)
        obj['bullr'+no+'e'].feedback = '=0'
      
      if (parseInt(obj['bullr'+no+'e'].value) == 0 && parseInt(obj['bullr'+no+'s'].value) != 0)
        obj['bullr'+no+'s'].feedback = '=0'
      
      if (parseInt(obj['bearr'+no+'s'].value) == 0 && parseInt(obj['bearr'+no+'e'].value) != 0)
        obj['bearr'+no+'e'].feedback = '=0'
      
      if (parseInt(obj['bearr'+no+'e'].value) == 0 && parseInt(obj['bearr'+no+'s'].value) != 0)
        obj['bearr'+no+'s'].feedback = '=0'
      
      // 清除一對
      if ((obj['bullr'+no+'s'].value.toString().length == 0 && obj['bullr'+no+'e'].value.toString().length == 0) ||
            (parseInt(obj['bullr'+no+'s'].value) == 0 && parseInt(obj['bullr'+no+'e'].value) == 0))
        obj['bullr'+no+'s'].feedback = '', obj['bullr'+no+'e'].feedback = ''
      
      if ((obj['bearr'+no+'s'].value.toString().length == 0 && obj['bearr'+no+'e'].value.toString().length == 0) ||
            (parseInt(obj['bearr'+no+'s'].value) == 0 && parseInt(obj['bearr'+no+'e'].value) == 0))
        obj['bearr'+no+'s'].feedback = '', obj['bearr'+no+'e'].feedback = ''
    }
    
    this.props.setStates({strategy1: obj})
  }
  
  handleBlur() {
    var {name, value} = event.target
    var states = this.props.getStates(),
        obj = $.extend(true, {}, states.strategy1)
        
    if (!obj[name].feedback) 
      // this.sendMsg(this, 'onChange', name)
      var a=1
  }
  
  handleClick() {
    event.preventDefault()
    var name = event.target.name
    this.sendMsg(this, 'onClick', name)
  }
  
  handleClick2() {
    var {name, checked} = event.target,
        no = event.target.attributes.getNamedItem('data-no').value
        
    this.sendMsg(this, 'onClick2', name)
  }
  
  sendMsg(that, handleType, name) {
    var states = this.props.getStates(),
        obj = $.extend(true, {}, states.strategy1)
    
    // btn
    if (handleType == 'onClick' && obj[name].value == false)
      obj[name].value = true
    else if (handleType == 'onClick' && obj[name].value == true)
      obj[name].value = false
    
    if (name == 'bullbuy' && obj.bullsell.value == true)
      obj.bullsell.value = false
    else if (name == 'bullsell' && obj.bullbuy.value == true)
      obj.bullbuy.value = false
    else if (name == 'bearbuy' && obj.bearsell.value == true)
      obj.bearsell.value = false
    else if (name == 'bearsell' && obj.bearbuy.value == true)
      obj.bearbuy.value = false
    
    // enable btn
    if (handleType == 'onClick2') {
      if (obj.enable.value == false) obj.enable.value = true
      else if (obj.enable.value == true) obj.enable.value = false
    }
    
    // send
    var command1 = {
      cmd: 'strategy1',
      algo_name: states.modules[Object.keys(states.modules)[0]],
      id : states.userId,
      ref: states.prefix+'0'
    }
    for (var [k, v] of Object.entries(obj)) {
      if (typeof v == 'object' && 'value' in v && v.value.toString().length > 0)
        command1[k] = v.value
      else if (typeof v == 'object' && 'value' in v && v.value.toString().length == 0)
        command1[k] = 0
    }
    sendWebsocket(JSON.stringify(command1))
    
    that.props.setStates({strategy1: obj})
  }
  
  getText(lang) {
    var text = {
      en: {buy: 'Buy ', sell: 'Sell ', bull: 'Bull', bear: 'Bear', empty: 'Cannot Empty', start: 'Start', end: 'End', on: 'On', off: 'Off'},
      sc: {buy: '买入', sell: '卖出', bull: '牛证', bear: '熊证', empty: '不能空', start: '上限', end: '下限', on: '启动', off: '停止'},
      tc: {buy: '買入', sell: '賣出', bull: '牛證', bear: '熊證', empty: '不能空', start: '上限', end: '下限', on: '啟動', off: '停止'}
    }
    return text[lang]
  }
  
  render() {
    var text = this.getText(this.props.lang),
        data = this.props.data

    // valid
    var isDisableBtn = false
    for(var [k2, v2] of Object.entries(data)) {
      if (typeof v2 == 'object' && ('feedback' in v2) && v2.feedback.toString().length>0)
        isDisableBtn = true
    }
    
    // input
    function getCell(i, that) {
      var keys = ['bullr'+i+'s', 'bullr'+i+'e', 'bearr'+i+'s', 'bearr'+i+'e'],
          placeholder = [text.buy+text.bull, text.buy+text.bull, text.buy+text.bear, text.buy+text.bear],
          rows1 = []
      
      for (var j in keys) {
        var k = keys[j]
        
        if (data[k].feedback)
          var style = 'is-invalid', feedback = <div className="invalid-feedback d-block">{data[k].feedback}</div>
        else if (data[k].responseResult)
          var style = 'is-valid', feedback = ''
        else
          var style = '', feedback = ''
        
        var s_or_e = k.substr(k.length - 1)
        
        if (s_or_e == 's')
          var placeholder1 = text.start
        else if (s_or_e = 'e')
          var placeholder1 = text.end
        
        rows1.push(
        <div key={'strategy1_'+i+'_'+k} className={"form-group col-3 col-md-1"} >
          <input
            data-no={i}
            type="text"
            name={k}
            className={classNames("form-control form-control-sm", style)}
            placeholder={placeholder[j]+' '+placeholder1+' '+i}
            onChange={that.handleChange}
            onBlur={that.handleBlur}
            value={data[k].value}
            autoComplete="off" />
          {feedback}
        </div>)
        
        // 
        if (s_or_e == 'e') {
          rows1.push(
            <div key={placeholder[j]+'-empty-'+i} className="d-none d-sm-block d-sm-none d-md-block col-md-2"></div>
          )
        }
        
      }
      
      var style2 = ''
      if (i>1) style2 = 'd-none'
      
      var html = 
      <div key={'strategy1_'+i} className={classNames("form-row mb-2", style2)} >
        {rows1}
        <div key={'strategy1_'+i+'_chxBox'} className={"form-group col-2 col-md-2"} >
          <div className="form-check">
          <input data-no={i} name="enable" className="form-check-input" type="checkbox" value="" onClick={that.handleClick2} disabled={isDisableBtn} defaultChecked={data.enable.value} />
          </div>
        </div>
      </div>
      return html
    }
    var rows = []
    for (var i=1; i<data.no+1; i++)
      rows.push(getCell(i, this))
    
    // btn
    var rows2 = []
    var keys2 = {
      bullbuy: text.buy+text.bull,
      bullsell: text.sell+text.bull,
      bearbuy: text.buy+text.bear,
      bearsell: text.sell+text.bear
    }
    
    for (var [k1, v1] of Object.entries(keys2)) {
      if (data[k1].value == true)
        var style1 = 'btn-success', text2 = text.on
      else if (data[k1].value == false)
        var style1 = 'btn-danger', text2 = text.off
      
      rows2.push(
        <div key={'strategy1_'+k1} className="form-group col-6 col-md-2 mb-2 mb-md-0">
        <button name={k1} className={classNames("btn btn-sm btn-block",style1)} onClick={this.handleClick} disabled={isDisableBtn} >{v1} ({text2})</button>
        </div>
      )
    }
    
    return(
<div className='row'>
<div className='col-12 mb-4 mb-md-2'>

<form>
<div className="form-row mb-2"> {rows2} </div> 
{rows}
</form>

</div>
</div>
    )
  }
}