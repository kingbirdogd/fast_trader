class IssuerSelector extends React.Component {
  static propTypes = {
    data: PropTypes.object,
    data2: PropTypes.number,
    data3: PropTypes.object,
    data4: PropTypes.object,
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
    this.props.func({issuer: event.target.value})
    //
    var states = this.props.getStates()
    var obj = $.extend(true, {}, states.issuer)
    obj.curIssuer = event.target.value
    obj.feedback = null
    obj.responseResult = null
    this.props.setStates({issuer: obj})
  }
  
  handleAction() {
    var action = event.target.name
    var states = this.props.getStates()
    var userId = parseInt(states.userId)
    var issuer = states.issuer.curIssuer
    var algoName = (states.modules.call) ? states.modules.call : states.modules.put
    
    if (action.toString().toLowerCase() == 'removeall')
      action = 'remove', issuer = 'ALL'
    
    var command = {cmd: 'selectissuer', algo_name: algoName, id: userId, ref: 'uid_'+userId.toString(), issuer: issuer, action: action}
    sendWebsocket(JSON.stringify(command))
  }
  
  getText(lang) {
    var text = {
      en: {select: 'Select', remove: 'Remove', selected: 'Selected', optionDefault: 'Select Issuer', removeAll: 'Remove All'},
      sc: {select: '选择', remove: '删除', selected: '已选择', optionDefault: '选择发行人', removeAll: '删除所有'},
      tc: {select: '選擇', remove: '刪除', selected: '已選擇', optionDefault: '選擇發行人', removeAll: '刪除所有'},
    }
    return text[lang]
  }
  
  render() {
    var {curIssuer, curState, feedback, responseResult, selected} = this.props.data
    var text = this.getText(this.props.lang)
    
    // 
    if (!curIssuer) var selectedValue='default'
    else if (curIssuer) var selectedValue=curIssuer
    
    // 
    var hint = null
    if (feedback)
      hint = <div className="text-danger"> {feedback} : {curState} </div>
    else if (responseResult)
      hint = <div className="text-success"> {responseResult} : {curState} </div>
    
    // 
    var optionHTML = []
    optionHTML.push(<option value="default" key='issuer_default' disabled> {text.optionDefault} </option>)
    for (const [k, v] of Object.entries(this.props.data4)) {
      var isSelect = '', style = ''
      if (selected && selected.includes(k))
        isSelect = text.selected, style = 'text-success'
      optionHTML.push(
        <option value={k} key={'issuer_'+k} className={style}>
          {k} ({v[this.props.lang]}) {isSelect}
        </option>)
    }
    
    // 
    if (this.props.data2 == 1 || curIssuer === null) {
      var btnSelectStyle = 'btn-secondary', btnSelectIsDisable = true
      var btnRemoveStyle = 'btn-secondary', btnRemoveIsDisable = true
    }
    else if (selected && selected.includes(curIssuer)) {
      var btnSelectStyle = 'btn-secondary', btnSelectIsDisable = true
      var btnRemoveStyle = 'btn-danger', btnRemoveIsDisable = false
    }
    else {
      var btnSelectStyle = 'btn-success', btnSelectIsDisable = false
      var btnRemoveStyle = 'btn-secondary', btnRemoveIsDisable = true
    }
    
    //
    var btnRemoveAllStyle = 'btn-secondary', btnRemoveAllIsDisable = false
    if (selected.length == 0)
      btnRemoveAllIsDisable = true
    
    var href = window.location.pathname, html1 = ''
    if (href.includes('a1')) {
      html1 = 
      <button
        name="removeAll"
        type="button"
        className={classNames('btn btn-sm', btnRemoveAllStyle)}
        disabled={btnRemoveAllIsDisable}
        onClick={this.handleAction}>
          {text.removeAll}
      </button>
    }
      
    return(
      <div className='row'>
      <div className="col-12 col-sm-6 col-md-6 mb-3">
      
      <div className="form-row">
      <div className="form-group col-8 col-sm-6 col-md-6">
        <select className="form-control" id="issuerSelector" onChange={this.handleChange} value={selectedValue}>
          {optionHTML}
        </select>
      </div>
      
      <div className="form-group col-4 col-sm-6 col-md-6">
        <WarrantTypeSelector
          key="warrantTypeSelector"
          data={this.props.data}
          data2={this.props.data3}
          lang={this.props.lang}
          setStates={this.props.setStates}
          getStates={this.props.getStates}
        />
      </div>
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
      
      {html1}
      
      {hint}
        
      </div>
      </div>
    )
  }
}