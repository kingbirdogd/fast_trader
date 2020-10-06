class WarrantSelector extends React.Component {
  static propTypes = {
    data: PropTypes.object,
    lang: PropTypes.string,
    setStates: PropTypes.func,
    getStates: PropTypes.func
  }
  
  constructor(props) {
    super(props)
    this.state = {}
    this.handleAction = this.handleAction.bind(this)
    this.handleChange = this.handleChange.bind(this)
  }
  
  static getDerivedStateFromProps(props, state) {
    return state
  }
  
  componentDidMount() {}
  
  handleChange() {
    var states = this.props.getStates()
    var obj = $.extend(true, {}, states.wntList)
    obj.wntCode = event.target.value
    obj.curState = null
    obj.feedback = null
    obj.responseResult = null
    this.props.setStates({wntList: obj})
  }
  
  handleAction() {
    var action = event.target.name
    var states = this.props.getStates()
    var userId = parseInt(states.userId)
    var wntCode = parseInt(this.props.data.wntCode)
    var ucode = parseInt(this.props.data.ucode)
    var algoName = (states.modules.call) ? states.modules.call : states.modules.put
    
    var command = {cmd: 'selectwarrant', algo_name: algoName, id: userId, ref: 'uid_'+userId.toString(), code: wntCode, action: action}
    sendWebsocket(JSON.stringify(command))
  }
  
  getText(lang) {
    var text = {
      en: {select: 'Select', selected: 'Selected', remove: 'Remove', removed: 'Removed'},
      sc: {select: '选择', selected: '已选择', remove: '删除', removed: '已删除'},
      tc: {select: '選擇', selected: '已選擇', remove: '刪除', removed: '已刪除'},
    }
    return text[lang]
  }
  
  render() {
    var text = this.getText(this.props.lang)
    
    //
    var data = this.props.data
    var rows = []
    if ('codes' in data) {
      for (var [k, v] of Object.entries(data.codes)) {
        if (v == 's')
          var status = text['selected'], style = 'text-success'
        else
          var status = '', style = ''
        
        rows.push(
          <option key={k} className={style} value={k}>
            {k} {status}
          </option>
        )
      }
    }
    
    //
    if (('wntCode' in data) && ('codes' in data) && data.codes[data.wntCode] == 'u') {
      var btnSelectIsDisable = false, btnSelectStyle = 'btn-success',
          btnRemoveIsDisable = true, btnRemoveStyle = 'btn-secondary'
    }
    else if (('wntCode' in data) && ('codes' in data) && data.codes[data.wntCode] == 's') {
      var btnSelectIsDisable = true, btnSelectStyle = 'btn-secondary',
          btnRemoveIsDisable = false, btnRemoveStyle = 'btn-danger'
    }
    else {
      var btnSelectIsDisable = true, btnSelectStyle = 'btn-secondary',
          btnRemoveIsDisable = true, btnRemoveStyle = 'btn-secondary'
    }
    
    //
    if (('wntCode' in data))
      var defaultValue = [data.wntCode]
    else
      var defaultValue = []
    
    //
    var hint = null
    if (('feedback' in data) && ('curState' in data) && data.feedback)
      hint = <div className="text-danger"> {data.feedback} : {data.curState} </div>
    else if (('responseResult' in data) && ('curState' in data) && data.responseResult)
      hint = <div className="text-success"> {data.responseResult} : {data.curState} </div>
    
    return(
      <div className='row'>
      <div className="col-12 col-sm-6 col-md-3 mb-3">
      
      <div className="form-group">
        <select multiple className="form-control" id="warrantSelector" onChange={this.handleChange} value={defaultValue}>
        {rows}
        </select>
      </div>
      
      <button
        name="select"
        type="button"
        className={classNames('btn btn-sm', btnSelectStyle)}
        disabled={btnSelectIsDisable}
        onClick={this.handleAction}>
          {text.select}
      </button>
      
      <button
        name="remove"
        type="button"
        className={classNames('btn btn-sm', btnRemoveStyle)}
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