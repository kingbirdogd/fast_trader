class Setting extends React.Component {
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
    this.handleReset = this.handleReset.bind(this)
    this.handleClick = this.handleClick.bind(this)
    this.handleChange = this.handleChange.bind(this)
  }
  
  handleReset() {
    var no = event.target.attributes.getNamedItem('data-no').value
    var states = this.props.getStates()
    var obj = $.extend(true, {}, states.data[no].setting)
    // 重设数据
    for (const [k, v] of Object.entries(obj)) {
      v.value = v.defaultValue
      v.feedback = ''
    }
    states.cells[no].setting = obj
    this.props.setStates({cells: states.cells})
  }
  
  handleClick() {
    var no = event.target.attributes.getNamedItem('data-no').value
    var {name, value} = event.target
    var states = this.props.getStates()
    var obj = $.extend(true, {}, states)
    var code = obj.cells[no].action.code.value
    var value = this.props.data2[name].value
    
    // 指令
    var version = 2
    if (version==1) {
      var command = ['param', code, name.toUpperCase(), value, obj.prefix+no]
      var command1 = {type: "algo_command", "key": obj.modules[this.props.type], command: command.join('|')}
    }
    else if (version==2) {
      var command1 = {
        cmd: "param",
        action: "param",
        warrant_code: parseInt(code),
        type: name.toUpperCase(),
        value: value,
        ref: obj.prefix+no,
        id: states.userId,
        algo_name: states.modules[this.props.type]
      }
    }
    sendWebsocket(JSON.stringify(command1))
  }
  
  handleChange() {
    var no = event.target.attributes.getNamedItem('data-no').value
    var {name, value} = event.target
    // 校验数据
    var states = this.props.getStates()
    var obj = $.extend(true, {}, states.cells[no].setting)
    obj[name].value = value
    obj[name].feedback = validate(this.props.lang, obj[name].valid, value, true)
    obj[name].responseResult = ''
    states.cells[no].setting = obj
    
    this.props.setStates({cells: states.cells})
  }
  
  getText(lang) {
    var text = {
      en: {
        inout: {label: 'In Out Range', placeholder: ''},
        wintick: {label: 'Win Ticks', placeholder: ''},
        inlvl: {label: 'In LvL Range', placeholder: ''},
        stoplost: {label: 'Stop Lost', placeholder: ''},
        ptrange: {label: 'Point Range', placeholder: ''},
        code: {label: 'Code', placeholder: ''},
        buyoffset: {label: 'Buy Offset', placeholder: ''},
        selloffset: {label: 'Sell Offset', placeholder: ''},
        priceTable: {label: 'Price Table'},
        showpt: {label: 'Show Point', placeholder: ''},
        lvlon: {label: 'Level On', placeholder: ''},
        rtData: {label: 'Real Time Data', placeholder: ''},
        btn: {set: 'Set', reset: 'Reset', on: 'On', off: 'Off'}
      },
      sc: {
        inout: {label: '价内/价外', placeholder: ''},
        wintick: {label: '止盈', placeholder: ''},
        inlvl: {label: '杠杆比率', placeholder: ''},
        stoplost: {label: '止蚀', placeholder: ''},
        ptrange: {label: '格数', placeholder: ''},
        code: {label: '牛熊证号码', placeholder: ''},
        buyoffset: {label: '买入偏移', placeholder: ''},
        selloffset: {label: '賣出偏移', placeholder: ''},
        priceTable: {label: '报价表'},
        showpt: {label: '颢示', placeholder: ''},
        lvlon: {label: '打和', placeholder: ''},
        rtData: {label: '即时数据', placeholder: ''},
        btn: {set: '确定', reset: '重设', on: '开启', off: '停止'}
      },
      tc: {
        inout: {label: '價內/價外', placeholder: ''},
        wintick: {label: '止盈', placeholder: ''},
        inlvl: {label: '槓桿比率', placeholder: ''},
        stoplost: {label: '止蝕', placeholder: ''},
        ptrange: {label: '格數', placeholder: ''},
        code: {label: '牛熊證號碼', placeholder: ''},
        buyoffset: {label: '買入偏移', placeholder: ''},
        selloffset: {label: '賣出偏移', placeholder: ''},
        priceTable: {label: '報價表'},
        showpt: {label: '顥示', placeholder: ''},
        lvlon: {label: '打和', placeholder: ''},
        rtData: {label: '即時數據', placeholder: ''},
        btn: {set: '確定', reset: '重設', on: '開啟', off: '停止'}
      }
    }
    return text[lang]
  }
  
  render() {
    var no = this.props.no
    var text = this.getText(this.props.lang)
    var isDisabled = (this.props.data1.code.value.length>0 && (this.props.data1.status.isSet || this.props.data1.status.isStart || this.props.data1.status.isPause || this.props.data1.status.isStop)) ? false : true
    return(
      <div className="tab-pane fade" id={"setting-"+no} role="tabpanel">
      
        {false &&
        <InputTextWithBtn 
          key={"inout_"+no}
          no={no}
          label={text.inout.label}
          name="inout"
          value={this.props.data2.inout.value}
          feedback={this.props.data2.inout.feedback}
          responseResult={this.props.data2.inout.responseResult}
          btnName={text.btn.set}
          isDisabled={isDisabled}
          isDisabledTv={false}
          onChange={this.handleChange}
          onClick={this.handleClick}
        />}
        
        <InputTextWithBtn 
          key={"wintick_"+no}
          no={no}
          label={text.wintick.label}
          name="wintick"
          value={this.props.data2.wintick.value}
          feedback={this.props.data2.wintick.feedback}
          responseResult={this.props.data2.wintick.responseResult}
          btnName={text.btn.set}
          isDisabled={isDisabled}
          isDisabledTv={false}
          onChange={this.handleChange}
          onClick={this.handleClick}
        />
        
        {false &&
        <InputTextWithBtn
          key={"inlvl_"+no}
          no={no}
          label={text.inlvl.label}
          name="inlvl"
          value={this.props.data2.inlvl.value}
          feedback={this.props.data2.inlvl.feedback}
          responseResult={this.props.data2.inlvl.responseResult}
          btnName={text.btn.set}
          isDisabled={isDisabled}
          isDisabledTv={false}
          onChange={this.handleChange}
          onClick={this.handleClick}
        />}
        
        <InputTextWithBtn
          key={"stoplost_"+no}
          no={no}
          label={text.stoplost.label}
          name="stoplost"
          value={this.props.data2.stoplost.value}
          feedback={this.props.data2.stoplost.feedback}
          responseResult={this.props.data2.stoplost.responseResult}
          btnName={text.btn.set}
          isDisabled={isDisabled}
          isDisabledTv={false}
          onChange={this.handleChange}
          onClick={this.handleClick}
        />
        
        {false &&
        <InputTextWithBtn
          key={"ptrange_"+no}
          no={no}
          label={text.ptrange.label}
          name="ptrange"
          value={this.props.data2.ptrange.value}
          feedback={this.props.data2.ptrange.feedback}
          responseResult={this.props.data2.ptrange.responseResult}
          btnName={text.btn.set}
          isDisabled={isDisabled}
          isDisabledTv={false}
          onChange={this.handleChange}
          onClick={this.handleClick}
        />}
        
        <InputTextWithBtn
          key={"buyoffset_"+no}
          no={no}
          label={text.buyoffset.label}
          name="buyoffset"
          value={this.props.data2.buyoffset.value}
          feedback={this.props.data2.buyoffset.feedback}
          responseResult={this.props.data2.buyoffset.responseResult}
          btnName={text.btn.set}
          isDisabled={isDisabled}
          isDisabledTv={false}
          onChange={this.handleChange}
          onClick={this.handleClick}
        />
        
        <InputTextWithBtn
          key={"selloffset_"+no}
          no={no}
          label={text.selloffset.label}
          name="selloffset"
          value={this.props.data2.selloffset.value}
          feedback={this.props.data2.selloffset.feedback}
          responseResult={this.props.data2.selloffset.responseResult}
          btnName={text.btn.set}
          isDisabled={isDisabled}
          isDisabledTv={false}
          onChange={this.handleChange}
          onClick={this.handleClick}
        />
        
        <InputToggle
          key={"showpt_toogle_"+no}
          no={no}
          label={text.showpt.label}
          name="showpt"
          value={this.props.data2.showpt.value}
          feedback={this.props.data2.showpt.feedback}
          responseResult={this.props.data2.showpt.responseResult}
          btnName1={text.btn.on}
          btnName2={text.btn.off}
          type={this.props.type}
          isDisabled={isDisabled}
          isDisabledTv={false}
          onChange={this.handleChange}
          onClick={this.handleClick}
          setStates={this.props.setStates}
          getStates={this.props.getStates}
        />
        
        <InputToggle
          key={"lvlon_toogle_"+no}
          no={no}
          label={text.lvlon.label}
          name="lvlon"
          value={this.props.data2.lvlon.value}
          feedback={this.props.data2.lvlon.feedback}
          responseResult={this.props.data2.lvlon.responseResult}
          btnName1={text.btn.on}
          btnName2={text.btn.off}
          type={this.props.type}
          isDisabled={isDisabled}
          isDisabledTv={false}
          onChange={this.handleChange}
          onClick={this.handleClick}
          setStates={this.props.setStates}
          getStates={this.props.getStates}
        />
        
        <InputToggle
          key={"rtData_"+no}
          no={no}
          label={text.rtData.label}
          name="rtData"
          value={this.props.data2.rtData.value}
          feedback={this.props.data2.rtData.feedback}
          responseResult={this.props.data2.rtData.responseResult}
          btnName1={text.btn.on}
          btnName2={text.btn.off}
          type={this.props.type}
          isDisabled={isDisabled}
          isDisabledTv={false}
          onChange={this.handleChange}
          onClick={this.handleClick}
          setStates={this.props.setStates}
          getStates={this.props.getStates}
        />
        
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
    
    /*<InputTextWithBtn
      key={"showpt_"+no}
      no={no}
      label={text.showpt.label}
      name="showpt"
      value={this.props.data2.showpt.value}
      feedback={this.props.data2.showpt.feedback}
      responseResult={this.props.data2.showpt.responseResult}
      btnName={text.btn.set}
      isDisabled={isDisabled}
      isDisabledTv={false}
      onChange={this.handleChange}
      onClick={this.handleClick}
    />*/
    
    /*<InputText 
      key={"code_t_"+no}
      no={no}
      label={text.code.label}
      name="code"
      value={this.props.data1.code.value}
      feedback={this.props.data1.code.feedback}
      disabled={true}
    />*/
    
    /*<div className="form-group row">
      <label className="col-4 col-sm-4 col-form-label col-form-label-sm"> {text.priceTable.label} </label>
      <div className="col-8 col-sm-8">
        <button 
          type="button"
          className="btn btn-sm btn-block btn-sm btn-warning"
          data-no={no}
          onClick={this.handleReset}>
            {text.btn.reset}
        </button>
      </div>
    </div>*/
  }
}