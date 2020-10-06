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
    
    var command = {cmd: 'selectunderlying', algo_name: algoName, id: userId, ref: 'uid_'+userId.toString(), ucode: ucode, action: action}
    sendWebsocket(JSON.stringify(command))
  }
  
  getText(lang) {
    var text = {
      en: {select: 'Select', remove: 'Remove', removed: 'Removed', optionDefault: 'Select Underlying'},
      sc: {select: '选择', remove: '删除', removed: '已删除', optionDefault: '选择相关资产'},
      tc: {select: '選擇', remove: '刪除', removed: '已刪除', optionDefault: '選擇相關資產'},
    }
    return text[lang]
  }
  
  render() {
    var text = this.getText(this.props.lang)
    var {curUnderlying, curState, feedback, responseResult, selected, removed} = this.props.data2
    var defaultOption = this.props.data3
    
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

    return(
      <div className='row'>
      <div className="col-12 col-sm-6 col-md-3 mb-3">
      
      <div className="form-group">
        <select className="form-control" id="underlyingSelector" onChange={this.handleChange} value={selectedValue}>
          {optionHTML}
        </select>
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
      
      {hint}
        
      </div>
      </div>
    )
  }
}