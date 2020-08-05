class UnderlyingSelector extends React.Component {
  static propTypes = {
    data: PropTypes.array,
    data2: PropTypes.object,
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
    var states = this.props.getStates()
    var obj = $.extend(true, {}, states.underlying)
    obj.curUnderlying = event.target.value
    obj.feedback = null
    obj.responseResult = null
    this.props.setStates({underlying: obj})
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
    var {curUnderlying, curState, feedback, responseResult, removed} = this.props.data2
    
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
    var ucodesName2 = {}
    for (const [k, v] of Object.entries(ucodesName))
      for (const [k1, v1] of Object.entries(v))
        ucodesName2[k1] = v1

    //
    var optionHTML = []
    optionHTML.push(<option value="default" key='underlying_default' disabled> {text.optionDefault} </option>)
    if (this.props.data) {
      for (var ucode of this.props.data) {
        var uname = (ucode in ucodesName2) ? ucodesName2[ucode] : ''
        var isRemove = '', style = ''
        if (removed && removed.includes(ucode))
          isRemove = text.removed, style = 'text-danger'
        optionHTML.push(
          <option value={ucode} key={'ucode_'+ucode} className={style}>
            {ucode} {uname} {isRemove}
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
    else if (removed && removed.includes(curUnderlying)) {
      var btnSelectStyle = 'btn-success', btnSelectIsDisable = false
      var btnRemoveStyle = 'btn-secondary', btnRemoveIsDisable = true
    }
    else {
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