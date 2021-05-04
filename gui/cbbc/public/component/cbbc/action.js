class Action extends React.Component {
  static propTypes = {
    no: PropTypes.string,
    type: PropTypes.string,
    data: PropTypes.object,
    isHide: PropTypes.object,
    lang: PropTypes.string,
    setStates: PropTypes.func,
    getStates: PropTypes.func
  }
  
  constructor(props) {
    super(props)
    this.handleChange = this.handleChange.bind(this)
    this.handleClick2 = this.handleClick2.bind(this)
  }
  
  handleChange() {
    var no = event.target.attributes.getNamedItem('data-no').value
    var {name, value} = event.target
    // 校验数据
    var states = this.props.getStates()
    var obj = $.extend(true, {}, states.cells[no])
    obj.action[name].value = formatInput(value)
    obj.action[name].feedback = validate(this.props.lang, obj.action[name].valid, value, false)
    // 如是输入框, 清除样式
    if ('responseResult' in obj.action[name])
      obj.action[name].responseResult = ''
    // 如stop后更改了data，必须重新set盘
    if (obj.action.status.isStop)
      obj.action.status.needToSetAgain = true
    // 如stop后更改了code，必须清price table
    if (obj.action.status.isStop && name == 'code')
      obj.priceTable = {}
    states.cells[no] = obj
    this.props.setStates({cells: states.cells})
  }
  
  handleClick2() {
    var no = event.target.attributes.getNamedItem('data-no').value
    var {name, value} = event.target
    
    var states = this.props.getStates()
    var obj = $.extend(true, {}, states.cells[no].action)
    var code = obj.code.value
    var value = this.props.data[name].value
    
    // 检验1
    var feedback = validate(this.props.lang, obj[name].valid, value, true)
    obj[name].feedback = feedback
    obj[name].responseResult = ''
    states.cells[no].action = obj

    if (feedback.length==0) {
      // 检验2
      var format1 = (val) => {return parseFloat(formatInputUnit(val, true))}
      if (name=='issuerSize')
        var val = format1(value)
      else if (name=='quantity')
        var val = formatLongV2(format1(value))
      // 错输入
      var version = 2
      if (!val) {
        obj[name].feedback = this.getText(this.props.lang).err.wrongNumber
        obj[name].responseResult = ''
        states.cells[no].action = obj
      }
      // 指令
      else if (val) {
        var keys = {issuerSize: 'ISIZE', quantity: 'QTY'}
        if (version==1) {
          var command = ['param', code, keys[name], val, states.prefix+no]
          var command1 = {type: "algo_command", "key": states.modules[this.props.type], command: command.join('|')}
        }
        else if (version==2) {
          var command1 = {
            cmd: "param",
            action: "param",
            warrant_code: parseInt(code),
            type: keys[name],
            value: val.toString(),
            ref: states.prefix+no,
            id: states.userId,
            algo_name: states.modules[this.props.type]
          }
        }
        sendWebsocket(JSON.stringify(command1))
      }
    }
    
    this.props.setStates({cells: states.cells})
  }
  
  getText(lang) {
    var text = {
      en: {
        code: {label: 'Code', placeholder: ''},
        symbol: {label: 'Symbol', placeholder: ''},
        issuerSize: {label: 'Issuer Size', placeholder: ''},
        quantity: {label: 'Quantity', placeholder: ''},
        spread: {label: 'Spread', placeholder: ''},
        delta: {label: 'Delta', placeholder: ''},
        btn: {set: 'Set'},
        err: {wrongNumber: 'Wrong Number Or Unit'}
      },
      sc: {
        code: {label: '牛熊证号码', placeholder: ''},
        symbol: {label: '商品代号', placeholder: ''},
        issuerSize: {label: '每手股数', placeholder: ''},
        quantity: {label: '申购份数', placeholder: ''},
        spread: {label: '格数', placeholder: ''},
        delta: {label: '对冲值', placeholder: ''},
        btn: {set: '设置'},
        err: {wrongNumber: '错误数字、单位'}
      },
      tc: {
        code: {label: '牛熊證號碼', placeholder: ''},
        symbol: {label: '商品代號', placeholder: ''},
        issuerSize: {label: '每手股數', placeholder: ''},
        quantity: {label: '買入額', placeholder: ''},
        spread: {label: '格數', placeholder: ''},
        delta: {label: '對沖值', placeholder: ''},
        btn: {set: '設置'},
        err: {wrongNumber: '錯誤數字、單位'}
      }
    }
    return text[lang]
  }
  
  render() {
    var no = this.props.no
    var type = this.props.type.charAt(0).toUpperCase()+this.props.type.slice(1)
    var text = this.getText(this.props.lang)
    
    // 是否已填滿所有選項
    var isFillAllData = true
    for (const [k, v] of Object.entries(this.props.data))
      if (v.hasOwnProperty('value') && v.value.toString().length<=0)
        isFillAllData = false
    
    var d = this.props.data.status
    var isTvDisabled = false
    // 如沒填滿, 停留set狀態
    if (!isFillAllData && (!d.isStart===true && !d.isPause===true))
      isTvDisabled = false
    // 如填滿, 等待start, 不能返回set狀態, 也不能再更改data
    else if (isFillAllData && d.isSet===true)
      isTvDisabled = true
    // 如填滿&啟動start, 等待pause/stop
    else if (isFillAllData && d.isStart===true)
      isTvDisabled = true
    // 等待pause, 返回set狀態, 重新開始
    else if (isFillAllData && d.isPause===true)
      isTvDisabled = true
    // 等待stop, 可返回set狀態更改Data, 也可start重新掛盤
    else if (d.isStop===true && d.needToSetAgain==false)
      isTvDisabled = false
    // 如stop后更改了data，必须重新set盘
    else if (d.isStop===true && d.needToSetAgain==true)
      isTvDisabled = false
    
    // 按鈕
    var isbBtnDisabled = true
    if(d.isPause===true)
      isbBtnDisabled = false
    else if (d.isSet===true || d.isStart===true || d.isStop===true)
      isbBtnDisabled = true
    
    // 文字框
    var isTvDisabled2 = true
    if (d.isSet===true || d.isStart===true)
      isTvDisabled2 = true
    else if (d.isPause===true || d.isStop===true || (d.isSet===false && d.isStart===false && d.isPause===false && d.isStop===false))
      isTvDisabled2 = false
    
    return(
      <div className="tab-pane fade show active" id={"type-"+no} role="tabpanel">
        
        <InputText 
          key={"code_"+no} 
          no={no} 
          label={text.code.label} 
          name="code" 
          value={this.props.data.code.value} 
          feedback={this.props.data.code.feedback} 
          disabled={isTvDisabled}
          onChange={this.handleChange}
        />
        
        <InputText
          key={"symbol_"+no}
          no={no}
          label={text.symbol.label}
          name="symbol"
          value={this.props.data.symbol.value}
          feedback={this.props.data.symbol.feedback}
          disabled={isTvDisabled}
          onChange={this.handleChange}
        />
        
        <InputTextWithBtn
          key={"issuerSize_"+no}
          no={no}
          label={text.issuerSize.label}
          name="issuerSize"
          value={this.props.data.issuerSize.value}
          feedback={this.props.data.issuerSize.feedback}
          responseResult={this.props.data.issuerSize.responseResult}
          btnName={text.btn.set}
          isDisabled={isbBtnDisabled}
          isDisabledTv={isTvDisabled2}
          onChange={this.handleChange}
          onClick={this.handleClick2}
        />
        
        <InputTextWithBtn
          key={"quantity_"+no}
          no={no}
          label={text.quantity.label}
          name="quantity"
          value={this.props.data.quantity.value}
          feedback={this.props.data.quantity.feedback}
          responseResult={this.props.data.quantity.responseResult}
          btnName={text.btn.set}
          isDisabled={isbBtnDisabled}
          isDisabledTv={isTvDisabled2}
          onChange={this.handleChange}
          onClick={this.handleClick2}
        />
        
        <InputText
          key={"spread_"+no}
          no={no}
          label={text.spread.label}
          name="spread"
          value={this.props.data.spread.value}
          feedback={this.props.data.spread.feedback}
          disabled={isTvDisabled}
          onChange={this.handleChange}
        />
        
        <InputText
          key={"delta_"+no}
          no={no}
          label={text.delta.label}
          name="delta"
          value={this.props.data.delta.value}
          feedback={this.props.data.delta.feedback}
          disabled={isTvDisabled}
          onChange={this.handleChange}
        />
        
        <BtnControl
          key={"btnControlAction_"+no}
          no={no}
          type={this.props.type}
          data={this.props.data}
          lang={this.props.lang}
          isHide={this.props.isHide}
          setStates={this.props.setStates}
          getStates={this.props.getStates}
        />
        
      </div>
    )
  }
  // 变成可以设置变量
  /*<InputText 
    key={"issuerSize_"+no}
    no={no}
    label={text.issuerSize.label}
    name="issuerSize"
    value={this.props.data.issuerSize.value}
    feedback={this.props.data.issuerSize.feedback}
    disabled={isTvDisabled}
    onChange={this.handleChange}
  />
  
  <InputText
    key={"quantity_"+no}
    no={no}
    label={text.quantity.label}
    name="quantity"
    value={this.props.data.quantity.value}
    feedback={this.props.data.quantity.feedback}
    disabled={isTvDisabled}
    onChange={this.handleChange}
  />*/
}