class UnderlyingSelector extends React.Component {
  static propTypes = {
    data: PropTypes.array,
    data2: PropTypes.object,
    data3: PropTypes.string,
    func: PropTypes.func,
    lang: PropTypes.string,
    setStates: PropTypes.func,
    getStates: PropTypes.func
  }
  
  constructor(props) {
    super(props)
    this.state = {}
    this.handleChange = this.handleChange.bind(this)
    this.handleAction = this.handleAction.bind(this)
    this.handleDetect = this.handleDetect.bind(this)
    this.handleChange2 = this.handleChange2.bind(this)
  }
  
  static getDerivedStateFromProps(props, state) {
    return state
  }
  
  componentDidMount() {}
  
  handleChange(event) {
    //
    this.props.func({ucode: event.target.value})
    //
    var states = this.props.getStates()
    var obj = $.extend(true, {}, states.underlying)
    obj.curUnderlying = event.target.value
    obj.feedback = null
    obj.responseResult = null
    this.props.setStates({underlying: obj})
    //
    var obj2 = $.extend(true, {}, states.wntList)
    obj2.wntCode = null
    obj2.curState = null
    obj2.feedback = null
    obj2.responseResult = null
    this.props.setStates({wntList: obj2})
  }
  
  handleAction() {
    var action = event.target.name
    var states = this.props.getStates()
    var userId = parseInt(states.userId)
    var ucode = parseInt(states.underlying.curUnderlying)
    var algoName = (states.modules.call) ? states.modules.call : states.modules.put
    
    if (action.toString().toLowerCase() == 'removeall')
      action = 'remove', ucode = 0
    else if (action.toString().toLowerCase() == 'selectall')
      action = 'select', ucode = 0
    
    var command = {cmd: 'selectunderlying', algo_name: algoName, id: userId, ref: 'uid_'+userId.toString(), ucode: ucode, action: action}
    sendWebsocket(JSON.stringify(command))
  }
  
  handleDetect() {
    var name = event.target.name
    var states = this.props.getStates()
    var userId = parseInt(states.userId)
    var algoName = (states.modules.call) ? states.modules.call : states.modules.put
    var obj1 = $.extend(true, {}, states.detectForce),
        obj2 = $.extend(true, {}, states.underlying)
    var detectprice = parseFloat(formatLongV2(obj1.price.value)), stoplost = parseFloat(formatLongV2(obj1.stoploss.value)), ucode = parseInt(obj2.curUnderlying)
    
    var command = {cmd: 'force_detect', algo_name: algoName, id: userId, ref: 'uid_'+userId.toString(), ucode: ucode, detectprice: detectprice, stoplost: stoplost}
    sendWebsocket(JSON.stringify(command))
  }
  
  handleChange2() {
    var {name, value} = event.target
    var states = this.props.getStates()

    var obj1 = $.extend(true, {}, states.detectForce)
    obj1[name].value = value
    obj1[name].feedback = validate(this.props.lang, obj1[name].valid, value, true)
    obj1[name].responseResult = ''
    this.props.setStates({detectForce: obj1})
  }
  
  getText(lang) {
    var text = {
      en: {select: 'Select', remove: 'Remove', removed: 'Removed', optionDefault: 'Select Underlying', removeAll: 'Remove All', selectAll: 'Select All', detect: 'Detect', price: 'Price', stoploss: 'Stop Loss'},
      sc: {select: '选择', remove: '删除', removed: '已删除', optionDefault: '选择相关资产', removeAll: '删除所有', selectAll: '选择所有', detect: '监察', price: '价格', stoploss: '止损点'},
      tc: {select: '選擇', remove: '刪除', removed: '已刪除', optionDefault: '選擇相關資產', removeAll: '刪除所有', selectAll: '選擇所有', detect: '監察', price: '價格', stoploss: '止損點'},
    }
    return text[lang]
  }
  
  render() {
    var text = this.getText(this.props.lang)
    var {curUnderlying, curState, feedback, responseResult, selected, removed} = this.props.data2
    var defaultOption = this.props.data3
    var data4 = this.props.data4
    
    // 
    if (!curUnderlying) var selectedValue='default'
    else if (curUnderlying) var selectedValue=curUnderlying
    
    // 
    var hint = null
    if (feedback)
      hint = <div className="text-danger"> {feedback} : {curState} </div>
    else if (responseResult)
      hint = <div className="text-success"> {responseResult} : {curState} </div>
    
    //
    var optionHTML = []
    optionHTML.push(<option value="default" key='underlying_default' disabled> {text.optionDefault} </option>)
    if (this.props.data) {
      for (var ucode of this.props.data) {
        var uname = getUnderlyingName2(ucode)
        
        var isSelect = '', isRemove = '', style = ''
        // 預設1 default select
        if (defaultOption == 'select' && selected && selected.includes(ucode))
          isSelect = text.select, style = 'text-success'
        // 預設2 default remove
        if (defaultOption == 'remove' && removed && removed.includes(ucode))
          isRemove = text.removed, style = 'text-danger'
        
        optionHTML.push(
          <option value={ucode} key={'ucode_'+ucode} className={style}>
            {ucode} {uname} {isSelect} {isRemove}
          </option>)
      }
    }
    
    //
    var btnSelectStyle = 'btn-secondary', btnSelectIsDisable = false
    var btnRemoveStyle = 'btn-secondary', btnRemoveIsDisable = false

    if (curUnderlying === null) {
      var btnSelectStyle = 'btn-secondary', btnSelectIsDisable = true
      var btnRemoveStyle = 'btn-secondary', btnRemoveIsDisable = true
    }
    // 預設1 default select
    else if (defaultOption == 'select' && selected && selected.includes(curUnderlying)) {
      var btnSelectStyle = 'btn-secondary', btnSelectIsDisable = true
      var btnRemoveStyle = 'btn-danger', btnRemoveIsDisable = false
    }
    else if (defaultOption == 'select') {
      var btnSelectStyle = 'btn-success', btnSelectIsDisable = false
      var btnRemoveStyle = 'btn-secondary', btnRemoveIsDisable = true
    }
    // 預設2 default remove
    else if (defaultOption == 'remove' && removed && removed.includes(curUnderlying)) {
      var btnSelectStyle = 'btn-success', btnSelectIsDisable = false
      var btnRemoveStyle = 'btn-secondary', btnRemoveIsDisable = true
    }
    else if (defaultOption == 'remove') {
      var btnSelectStyle = 'btn-secondary', btnSelectIsDisable = true
      var btnRemoveStyle = 'btn-danger', btnRemoveIsDisable = false
    }
    
    //
    var btnRemoveAllStyle = 'btn-secondary', btnRemoveAllIsDisable = false
    var btnSelectAllStyle = 'btn-secondary', btnSelectAllIsDisable = false
    
    // a1 only
    var href = window.location.pathname, html1 = '', html2 = ''
    if (href.includes('a1')) {
      var btnDetectIsDisable = false, styleDetectPrice = '', styleDetectStoploss = ''
      if (data4.price.value.toString().length == 0 || data4.stoploss.value.toString().length == 0 || 
          data4.price.feedback.toString().length > 0 || data4.stoploss.feedback.toString().length > 0 ||
          curUnderlying === null || curUnderlying.toString().length == 0)
        btnDetectIsDisable = true
      if (data4.price.feedback.toString().length > 0)
        styleDetectPrice = 'is-invalid'
      if (data4.stoploss.feedback.toString().length > 0)
        styleDetectStoploss = 'is-invalid'
      
      html1 = 
      <React.Fragment>
      <button
        name="selectAll"
        type="button"
        className={classNames('btn btn-sm', btnSelectAllStyle)}
        disabled={btnSelectAllIsDisable}
        onClick={this.handleAction}>
          {text.selectAll}
      </button>
      
      <button
        name="removeAll"
        type="button"
        className={classNames('btn btn-sm', btnRemoveAllStyle)}
        disabled={btnRemoveAllIsDisable}
        onClick={this.handleAction}>
          {text.removeAll}
      </button>
      </React.Fragment>
      
      html2 =
      <React.Fragment>
      <div className="form-group col-3 col-sm-2 col-md-2">
      <input
        name="price"
        className={classNames("form-control", styleDetectPrice)}
        onChange={this.handleChange2}
        value={data4.price.value}
        placeHolder={text.price}
        autoComplete="off"/>
      </div>
      
      <div className="form-group col-3 col-sm-2 col-md-2">
      <input
        name="stoploss"
        className={classNames("form-control", styleDetectStoploss)}
        onChange={this.handleChange2}
        value={data4.stoploss.value}
        placeHolder={text.stoploss}
        autoComplete="off"/>
      </div>
      
      <div className="form-group col-1 col-sm-1 col-md-1">
      <button
        name="detect"
        className={classNames('btn btn-sm btn-secondary', '')}
        disabled={btnDetectIsDisable}
        onClick={this.handleDetect}>
          {text.detect}
      </button>
      </div>
      </React.Fragment>
    }

    return(
      <div className='row'>
      <div className="col-12 col-sm-12 col-md-12 mb-3">
      
      <div className="form-row">
        <div className="form-group col-4 col-sm-3 col-md-3">
          <select className="form-control" id="underlyingSelector" onChange={this.handleChange} value={selectedValue}>
            {optionHTML}
          </select>
        </div>
        {html2}
      </div>
      
      <button
        name="select"
        type="button"
        className={classNames('btn btn-sm btn-secondary', btnSelectStyle)}
        disabled={btnSelectIsDisable}
        onClick={this.handleAction}>
          {text.select}
      </button>
      
      <button
        name="remove"
        type="button"
        className={classNames('btn btn-sm btn-secondary', btnRemoveStyle)}
        disabled={btnRemoveIsDisable}
        onClick={this.handleAction}>
          {text.remove}
      </button>
      
      {html1}
      
      {hint}
        
      </div>
      </div>
    )
  }
}