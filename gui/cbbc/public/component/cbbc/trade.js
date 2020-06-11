class Trade extends React.Component {
  static propTypes = {
    no: PropTypes.string,
    data1: PropTypes.object,
    data2: PropTypes.object,
    isHide: PropTypes.object,
    type: PropTypes.string,
    lang: PropTypes.string,
    setStates: PropTypes.func,
    getStates: PropTypes.func
  }
  
  constructor(props) {
    super(props)
    this.handleClick = this.handleClick.bind(this)
    this.handleChange = this.handleChange.bind(this)
  }
  
  handleClick() {
    var no = event.target.attributes.getNamedItem('data-no').value
    var name = event.target.name
    var states = this.props.getStates()
    var obj = $.extend(true, {}, states.cells[no].trade)
    obj.code = $.extend(true, {}, states.cells[no].action.code)
    
    // 校验数据
    var isError = false
    for (const [k, v] of Object.entries(obj)) {
      if(v.hasOwnProperty("valid")) {
        v.feedback = validate(this.props.lang, v.valid, v.value, true)
        if (v.feedback.length>0)
          isError = true
      }
    }
    
    // 指令
    var cAction = {buy: 'force_buy', sell: 'force_sell', position: 'set_position'}
    var version = 2
    if (!isError && (name in cAction)) {
      if(version==1) {
        var command = [cAction[name], formatLongV2(obj.price.value), formatLongV2(formatInputUnit(obj.size.value, true)), states.prefix+no]
        var command1 = {type: "algo_command", "key": states.modules[this.props.type], command: command.join('|')}
      }
      else if (version==2) {
        var command1 = {
          cmd: cAction[name],
          warrant_code: parseInt(obj.code.value),
          price: formatLongV2(obj.price.value),
          quantity: formatLongV2(formatInputUnit(obj.size.value, true)),
          ref: states.prefix+no,
          algo_name: states.modules[this.props.type]
        }
      }
      sendWebsocket(JSON.stringify(command1))
    }
    
    // 更新校验
    states.cells[no].action.code = obj.code
    states.cells[no].trade.price = obj.price
    states.cells[no].trade.size = obj.size
    this.props.setStates({cells: states.cells})
  }
  
  handleChange() {
    var no = event.target.attributes.getNamedItem('data-no').value
    var {name, value} = event.target
    // 校验数据
    var states = this.props.getStates()
    var obj = $.extend(true, {}, states.cells[no].trade)
    obj[name].value = value
    obj[name].feedback = validate(this.props.lang, obj[name].valid, value, false)
    states.cells[no].trade = obj
    this.props.setStates(states)
  }
  
  getText(lang) {
    var text = {
      en: {
        btn: {buy: 'Buy', sell: 'Sell', position: 'Set Position'},
        code: {label: 'Code', placeholder: ''},
        price: {label: 'Price', placeholder: ''},
        size: {label: 'Size', placeholder: ''},
        position: {label: 'Position', placeholder: ''}
      },
      sc: {
        btn: {buy: '买入', sell: '卖出', position: '设置手持仓位'},
        code: {label: '牛熊证号码', placeholder: ''},
        price: {label: '现价', placeholder: ''},
        size: {label: '申购份数', placeholder: ''},
        position: {label: '预设持仓', placeholder: ''}
      },
      tc: {
        btn: {buy: '買入', sell: '賣出', position: '設置手持倉位'},
        code: {label: '牛熊證號碼', placeholder: ''},
        price: {label: '現價', placeholder: ''},
        size: {label: '買入額', placeholder: ''},
        position: {label: '預設持倉', placeholder: ''}
      },
    }
    return text[lang]
  }
  
  render() {
    var no = this.props.no
    var text = this.getText(this.props.lang)
    
    // 如有错误数据，禁止按钮
    var isBtnDisabled = false, isBtnDisabled2 = false
    for (const [k, v] of Object.entries(this.props.data2)) {
      if(v.hasOwnProperty("feedback") && v.feedback.length>0)
        isBtnDisabled = true, isBtnDisabled2 = true
    }
    // 如沒set or 已stop, 禁止直接買賣
    var s = this.props.data1.status
    if ((!s.isPause && !s.isSet && !s.isStart && !s.isStop) || s.isStop)
      isBtnDisabled = true
    
    // 只有pause & stop才可以set position
    if ((s.isStart || s.isSet) || this.props.data1.code.value.length<=0)
      isBtnDisabled2 = true
      
    // 样式
    var style = ['btn-primary', 'btn-success']
    
    return(
      <div className="tab-pane fade" id={"trade-"+no} role="tabpanel">

        <InputText 
          key={"code_t_"+no}
          no={no}
          label={text.code.label}
          name="code"
          value={this.props.data1.code.value}
          feedback={this.props.data1.code.feedback}
          disabled={true}
        />
        
        <InputText
          key={"price_"+no}
          no={no}
          label={text.price.label}
          name="price"
          value={this.props.data2.price.value}
          feedback={this.props.data2.price.feedback}
          onChange={this.handleChange}
        />
        
        <InputText
          key={"size_"+no}
          no={no}
          label={text.size.label}
          name="size"
          value={this.props.data2.size.value}
          feedback={this.props.data2.size.feedback}
          onChange={this.handleChange}
        />
        
        <div className="form-group row">
          <div className="col-5 col-sm-5 mt-0 mt-sm-0"></div>
          <div className="col-7 col-sm-7 mt-0 mt-sm-0">
            <button 
              name="position"
              type="button"
              className="btn btn-sm btn-block btn-secondary"
              data-no={no}
              disabled={isBtnDisabled2}
              onClick={this.handleClick}>
                {text.btn.position}
            </button>
          </div>
        </div>
        
        <div className="btn_trade_control">
          <div className="form-group row">
            <div className="col-6 col-sm-6 mt-0 mt-sm-0">
              <button 
                name="buy"
                type="button"
                className="btn btn-sm btn-block btn-secondary"
                data-no={no}
                disabled={isBtnDisabled}
                onClick={this.handleClick}>
                  {text.btn.buy}
              </button>
            </div>
            
            <div className="col-6 col-sm-6 mt-0 mt-sm-0">
              <button
                name="sell"
                type="button"
                className="btn btn-sm btn-block btn-secondary"
                data-no={no}
                disabled={isBtnDisabled}
                onClick={this.handleClick}>
                  {text.btn.sell}
              </button>
            </div>
          </div>
        </div>
        
        <BtnControl
          key={"btnControlPriceTb_"+no}
          no={no}
          type={this.props.type}
          data={this.props.data1}
          lang={this.props.lang}
          isHide={this.props.isHide}
          setStates={this.props.setStates}
          getStates={this.props.getStates}
        />
        
      </div>
    )
  }
}